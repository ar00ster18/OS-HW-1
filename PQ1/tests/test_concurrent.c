#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include "tl_semaphore.h"

/* Concurrency test: 8 threads each call semaphore_wait() once.
 * Main thread calls semaphore_signal() 8 times.
 * Verifies all 8 threads finish. */

#define NUM_THREADS 8

static semaphore sem;
static atomic_int finished_count;

static void* thread_func(void* arg)
{
    (void)arg;
    semaphore_wait(&sem);
    atomic_fetch_add(&finished_count, 1);
    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    atomic_init(&finished_count, 0);
    semaphore_init(&sem, 0);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, thread_func, NULL) != 0)
        {
            fprintf(stderr, "FAIL: pthread_create failed\n");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        semaphore_signal(&sem);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    int count = atomic_load(&finished_count);
    if (count != NUM_THREADS)
    {
        fprintf(stderr, "FAIL: expected %d threads to finish, got %d\n", NUM_THREADS, count);
        return 1;
    }

    if (sem.value != 0)
    {
        fprintf(stderr, "FAIL: expected final value 0, got %d\n", sem.value);
        return 1;
    }

    printf("PASS: all %d threads finished, final value == 0\n", NUM_THREADS);
    return 0;
}
