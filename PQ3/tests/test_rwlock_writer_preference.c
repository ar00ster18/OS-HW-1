#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <stdatomic.h>

#include "../rw_lock.h"


rwlock lock;

/*
    Used to verify writer preference.

    If writer preference works correctly:
    - reader 2 should NOT enter
      before writer acquires/releases lock.
*/

int writer_entered = 0;
int reader2_entered = 0;


/* first reader */
void* reader1_thread(void* arg)
{
    (void)arg;

    rwlock_acquire_read(&lock);

    /* hold read lock for a while */
    sleep(3);

    rwlock_release_read(&lock);

    return NULL;
}


/* writer thread */
void* writer_thread(void* arg)
{
    (void)arg;

    /*
        Give reader1 time to acquire first
    */
    sleep(5);

    rwlock_acquire_write(&lock);

    writer_entered = 1;

    /* hold write lock briefly */
    sleep(10);

    rwlock_release_write(&lock);

    return NULL;
}


/* second reader */
void* reader2_thread(void* arg)
{
    (void)arg;

    /*
        Ensure writer is already waiting
        before reader2 attempts entry
    */
    sched_yield();

    rwlock_acquire_read(&lock);

    reader2_entered = 1;

    /*
        If reader2 enters BEFORE writer,
        writer preference failed
    */
    if (writer_entered == 0)
    {
        printf("FAIL: reader entered before waiting writer\n");
        exit(1);
    }

    rwlock_release_read(&lock);

    return NULL;
}


int main(void)
{
    pthread_t r1;
    pthread_t r2;
    pthread_t w;

    rwlock_init(&lock);

    pthread_create(&r1, NULL, reader1_thread, NULL);
    pthread_create(&w,  NULL, writer_thread,  NULL);
    pthread_create(&r2, NULL, reader2_thread, NULL);

    pthread_join(r1, NULL);
    pthread_join(w,  NULL);
    pthread_join(r2, NULL);

    if (writer_entered && reader2_entered)
    {
        printf("PASS: writer preference enforced\n");
        return 0;
    }

    printf("FAIL: test incomplete\n");
    return 1;
}