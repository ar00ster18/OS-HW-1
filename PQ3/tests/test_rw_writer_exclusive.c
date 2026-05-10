#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>
#include "rw_lock.h"

/* Writer-exclusivity test (1 writer + 8 readers):
 * The writer acquires the write lock and checks that readers_in_cs == 0,
 * verifying no reader is inside the critical section simultaneously.
 * Readers repeatedly enter/exit the read section. */

#define NUM_READERS       8
#define WRITER_ITERATIONS 100
#define READER_ITERATIONS 500

static rwlock lock;
static atomic_int readers_in_cs;
static atomic_int violation_detected;

static void* reader_thread(void* arg)
{
    (void)arg;

    for (int i = 0; i < READER_ITERATIONS; i++)
    {
        rwlock_acquire_read(&lock);
        atomic_fetch_add(&readers_in_cs, 1);
        sched_yield();
        atomic_fetch_sub(&readers_in_cs, 1);
        rwlock_release_read(&lock);
    }

    return NULL;
}

static void* writer_thread(void* arg)
{
    (void)arg;

    for (int i = 0; i < WRITER_ITERATIONS; i++)
    {
        rwlock_acquire_write(&lock);

        /* while holding write lock, no reader should be inside */
        if (atomic_load(&readers_in_cs) != 0)
        {
            atomic_store(&violation_detected, 1);
        }

        sched_yield();
        rwlock_release_write(&lock);
    }

    return NULL;
}

int main(void)
{
    pthread_t readers[NUM_READERS];
    pthread_t writer;

    rwlock_init(&lock);
    atomic_init(&readers_in_cs, 0);
    atomic_init(&violation_detected, 0);

    for (int i = 0; i < NUM_READERS; i++)
    {
        if (pthread_create(&readers[i], NULL, reader_thread, NULL) != 0)
        {
            fprintf(stderr, "FAIL: pthread_create failed\n");
            return 1;
        }
    }

    if (pthread_create(&writer, NULL, writer_thread, NULL) != 0)
    {
        fprintf(stderr, "FAIL: pthread_create failed\n");
        return 1;
    }

    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writer, NULL);

    if (atomic_load(&violation_detected))
    {
        fprintf(stderr, "FAIL: writer overlapped with a reader\n");
        return 1;
    }

    printf("PASS: writer was always exclusive (no reader overlap detected)\n");
    return 0;
}
