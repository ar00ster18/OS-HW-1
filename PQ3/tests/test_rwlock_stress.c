#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../rw_lock.h"


#define NUM_READERS 8
#define NUM_WRITERS 4
#define NUM_ITERATIONS 100


rwlock lock;

/* shared state for validation */
int active_readers = 0;
int active_writers = 0;


/* reader thread */
void* reader_thread(void* arg)
{
    (void)arg;

    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        rwlock_acquire_read(&lock);

        /*
            Readers must never overlap with writers
        */
        if (active_writers != 0)
        {
            printf("FAIL: reader overlapped with writer\n");
            exit(1);
        }

        active_readers++;

        /* small delay increases concurrency chance */
        sleep(1);

        active_readers--;

        rwlock_release_read(&lock);
    }

    return NULL;
}


/* writer thread */
void* writer_thread(void* arg)
{
    (void)arg;

    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        rwlock_acquire_write(&lock);

        /*
            Writers must be completely exclusive
        */
        if (active_writers != 0 ||
            active_readers != 0)
        {
            printf("FAIL: writer exclusivity violated\n");
            exit(1);
        }

        active_writers = 1;

        /* small delay increases concurrency chance */
        sleep(1);

        active_writers = 0;

        rwlock_release_write(&lock);
    }

    return NULL;
}


int main(void)
{
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    rwlock_init(&lock);

    /* create reader threads */
    for (int i = 0; i < NUM_READERS; i++)
    {
        if (pthread_create(&readers[i],
                           NULL,
                           reader_thread,
                           NULL) != 0)
        {
            perror("pthread_create");
            return 1;
        }
    }

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

    /* wait for readers */
    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    /* wait for writers */
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_join(writers[i], NULL);
    }

    printf("PASS: stress test completed successfully\n");

    return 0;
}