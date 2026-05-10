#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include "rw_lock.h"

/* Readers-parallel test (8 threads):
 * Verifies that multiple readers can hold the lock simultaneously.
 * Uses sleep(1) to force overlap between readers. */

#define NUM_READERS 8

static rwlock lock;
static atomic_int readers_inside;
static atomic_int max_simultaneous;

static void update_max(int n)
{
    int old = atomic_load(&max_simultaneous);
    while (n > old)
    {
        if (atomic_compare_exchange_weak(&max_simultaneous, &old, n)) break;
    }
}

static void* reader_thread(void* arg)
{
    (void)arg;

    rwlock_acquire_read(&lock);

    int n = atomic_fetch_add(&readers_inside, 1) + 1;
    update_max(n);

    sleep(1);  /* hold long enough to overlap with other readers */

    atomic_fetch_sub(&readers_inside, 1);
    rwlock_release_read(&lock);

    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_READERS];

    rwlock_init(&lock);
    atomic_init(&readers_inside, 0);
    atomic_init(&max_simultaneous, 0);

    for (int i = 0; i < NUM_READERS; i++)
    {
        if (pthread_create(&threads[i], NULL, reader_thread, NULL) != 0)
        {
            fprintf(stderr, "FAIL: pthread_create failed\n");
            return 1;
        }
    }

    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    int max = atomic_load(&max_simultaneous);
    if (max < 2)
    {
        fprintf(stderr, "FAIL: max simultaneous readers was %d, expected >= 2\n", max);
        return 1;
    }

    printf("PASS: %d readers were inside simultaneously\n", max);
    return 0;
}
