#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>
#include "cond_var.h"

/* Broadcast stress test (16 threads): each thread repeatedly waits on the CV.
 * Main broadcasts to wake all of them each iteration.
 * Verifies no deadlock and that all 16 threads are released each iteration. */

#define NUM_THREADS  16
#define ITERATIONS   100

static condition_variable cv;
static ticket_lock ext_lock;
static atomic_int released_count;

static void* thread_func(void* arg)
{
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++)
    {
        ticketlock_acquire(&ext_lock);
        condition_variable_wait(&cv, &ext_lock);
        atomic_fetch_add(&released_count, 1);
        ticketlock_release(&ext_lock);
    }
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

    for (int iter = 0; iter < ITERATIONS; iter++)
    {
        /* Wait until all threads are registered as waiting */
        while (atomic_load(&cv.num_waiters) < NUM_THREADS)
        {
            sched_yield();
        }

        int before = atomic_load(&released_count);
        condition_variable_broadcast(&cv);

        /* Wait for all threads to wake up */
        while (atomic_load(&released_count) < before + NUM_THREADS)
        {
            sched_yield();
        }

        int released = atomic_load(&released_count) - before;
        if (released != NUM_THREADS)
        {
            fprintf(stderr, "FAIL: iteration %d: expected %d released, got %d\n",
                    iter, NUM_THREADS, released);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("PASS: %d iterations x %d threads, all woken by broadcast each time\n",
           ITERATIONS, NUM_THREADS);
    return 0;
}
