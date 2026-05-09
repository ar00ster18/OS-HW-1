#include <stdio.h>
#include <pthread.h>
#include "tl_semaphore.h"

/* Stress test: 16 threads each perform 10,000 wait/signal pairs.
 * Verifies no deadlock and that the final semaphore value matches the initial value. */

#define NUM_THREADS  16
#define OPS_PER_THREAD 10000
#define INITIAL_VALUE 4

static semaphore sem;

static void* thread_func(void* arg)
{
    (void)arg;
    for (int i = 0; i < OPS_PER_THREAD; i++)
    {
        semaphore_wait(&sem);
        semaphore_signal(&sem);
    }
    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    semaphore_init(&sem, INITIAL_VALUE);

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
        pthread_join(threads[i], NULL);
    }

    if (sem.value != INITIAL_VALUE)
    {
        fprintf(stderr, "FAIL: expected final value %d, got %d\n", INITIAL_VALUE, sem.value);
        return 1;
    }

    printf("PASS: stress test completed, %d threads x %d ops, final value == %d\n",
           NUM_THREADS, OPS_PER_THREAD, sem.value);
    return 0;
}
