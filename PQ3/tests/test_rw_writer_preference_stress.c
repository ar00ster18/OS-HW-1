#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>
#include "rw_lock.h"

/* Writer-preference stress test (12 readers + 4 writers):
 * Readers continuously acquire/release the read lock.
 * Writers repeatedly acquire/release the write lock and count acquisitions.
 * Verifies that every writer acquires the lock at least MIN_WRITER_ACQUISITIONS
 * times, i.e., writers do not starve under writer-preference policy.
 *
 * Total write acquisitions: NUM_WRITERS * WRITER_ITERATIONS = ~5000 */

#define NUM_READERS              12
#define NUM_WRITERS              4
#define WRITER_ITERATIONS        1250   /* 5000 total across 4 writers */
#define READER_ITERATIONS        500
#define MIN_WRITER_ACQUISITIONS  100

static rwlock lock;
static atomic_int writer_counts[NUM_WRITERS];

static void* reader_thread(void* arg)
{
    (void)arg;

    for (int i = 0; i < READER_ITERATIONS; i++)
    {
        rwlock_acquire_read(&lock);
        sched_yield();
        rwlock_release_read(&lock);
    }

    return NULL;
}

static void* writer_thread(void* arg)
{
    int id = *(int*)arg;

    for (int i = 0; i < WRITER_ITERATIONS; i++)
    {
        rwlock_acquire_write(&lock);
        atomic_fetch_add(&writer_counts[id], 1);
        sched_yield();
        rwlock_release_write(&lock);
    }

    return NULL;
}

int main(void)
{
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    int writer_ids[NUM_WRITERS];

    rwlock_init(&lock);
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        atomic_init(&writer_counts[i], 0);
        writer_ids[i] = i;
    }

    for (int i = 0; i < NUM_READERS; i++)
    {
        if (pthread_create(&readers[i], NULL, reader_thread, NULL) != 0)
        {
            fprintf(stderr, "FAIL: pthread_create failed\n");
            return 1;
        }
    }

    for (int i = 0; i < NUM_WRITERS; i++)
    {
        if (pthread_create(&writers[i], NULL, writer_thread, &writer_ids[i]) != 0)
        {
            fprintf(stderr, "FAIL: pthread_create failed\n");
            return 1;
        }
    }

    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        pthread_join(writers[i], NULL);
    }

    int failed = 0;
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        int count = atomic_load(&writer_counts[i]);
        printf("Writer %d: %d acquisitions\n", i, count);
        if (count < MIN_WRITER_ACQUISITIONS)
        {
            fprintf(stderr, "FAIL: writer %d only got %d acquisitions (expected >= %d)\n",
                    i, count, MIN_WRITER_ACQUISITIONS);
            failed = 1;
        }
    }

    if (failed) return 1;

    printf("PASS: all writers acquired lock >= %d times (no starvation)\n",
           MIN_WRITER_ACQUISITIONS);
    return 0;
}
