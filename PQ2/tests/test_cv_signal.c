#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>
#include "cond_var.h"

/* Signal test (8 threads): each thread waits on the CV once.
 * Main signals exactly 8 times, one at a time.
 * Verifies that after the i-th signal, exactly i threads have been released. */

#define NUM_THREADS 8

static condition_variable cv;
static ticket_lock ext_lock;
static atomic_int released_count;

static void* thread_func(void* arg)
{
    (void)arg;
    ticketlock_acquire(&ext_lock);
    condition_variable_wait(&cv, &ext_lock);
    atomic_fetch_add(&released_count, 1);
    ticketlock_release(&ext_lock);
    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];

    condition_variable_init(&cv);
    ticketlock_init(&ext_lock);
    atomic_init(&released_count, 0);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, thread_func, NULL) != 0)
        {
            fprintf(stderr, "FAIL: pthread_create failed\n");
            return 1;
        }
    }

    /* Wait until all threads are registered as waiting */
    while (atomic_load(&cv.num_waiters) < NUM_THREADS)
    {
        sched_yield();
    }

    /* Signal one at a time, verify exactly one thread wakes each time */
    for (int i = 1; i <= NUM_THREADS; i++)
    {
        condition_variable_signal(&cv);

        /* Wait for one more thread to be released */
        while (atomic_load(&released_count) < i)
        {
            sched_yield();
        }

        if (atomic_load(&released_count) != i)
        {
            fprintf(stderr, "FAIL: after signal %d, expected %d released, got %d\n",
                    i, i, atomic_load(&released_count));
            return 1;
        }

        printf("PASS: after signal %d, exactly %d thread(s) released\n", i, i);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("All signal tests passed.\n");
    return 0;
}
