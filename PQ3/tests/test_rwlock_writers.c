#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../rw_lock.h"


#define NUM_WRITERS 6


rwlock lock;

/* number of writers currently inside critical section */
int active_writers = 0;

/* tracks whether exclusivity was violated */
int violation_detected = 0;


/* writer thread */
void* writer_thread(void* arg)
{
    (void)arg;

    rwlock_acquire_write(&lock);

    /*
        Only ONE writer should ever
        hold the lock at once.
    */
    active_writers++;

    if (active_writers > 1)
    {
        violation_detected = 1;
    }

    /* force overlap attempt */
    sleep(1);

    active_writers--;

    rwlock_release_write(&lock);

    return NULL;
}


int main(void)
{
    pthread_t writers[NUM_WRITERS];

    rwlock_init(&lock);

    /* create writer threads */
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        if (pthread_create(&writers[i],
                           NULL,
                           writer_thread,
                           NULL) != 0)
        {
            perror("pthread_create");
            return 1;
        }
    }

    /* wait for all writers */
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_join(writers[i], NULL);
    }

    /*
        If exclusivity worked correctly,
        active_writers should never exceed 1.
    */
    if (violation_detected)
    {
        printf("FAIL: multiple writers entered simultaneously\n");
        return 1;
    }

    printf("PASS: writers were mutually exclusive\n");

    return 0;
}