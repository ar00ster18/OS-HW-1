#ifndef RW_LOCK_H
#define RW_LOCK_H

#include "tl_semaphore.h"
#include "cond_var.h"


/* =========================
   READERS-WRITER LOCK
   ========================= */

/*
 * Writer-preference readers-writer lock.
 *
 * Guarantees:
 *   - Multiple readers may hold the lock simultaneously.
 *   - A writer requires exclusive access (no readers or other writers).
 *   - If any writer is waiting, new readers are blocked (writer preference),
 *     preventing writer starvation.
 */
typedef struct
{
    ticket_lock lock;              /* protects all fields below */
    condition_variable readers_cv; /* readers wait here when a writer holds or is waiting */
    condition_variable writers_cv; /* writers wait here when readers or a writer is active */
    int active_readers;            /* number of readers currently holding the lock */
    int waiting_writers;           /* number of writers waiting to acquire */
    int active_writer;             /* 1 if a writer currently holds the lock, 0 otherwise */

} rwlock;


/**
 * Initializes the RW-lock.
 * Must be called before any other operation.
 */
void rwlock_init(rwlock* lock);

/**
 * Acquires the lock for reading.
 * Blocks if a writer holds the lock or any writer is waiting (writer preference).
 */
void rwlock_acquire_read(rwlock* lock);

/**
 * Releases the read lock.
 * If this was the last active reader and writers are waiting, wakes one writer.
 */
void rwlock_release_read(rwlock* lock);

/**
 * Acquires the lock for writing (exclusive access).
 * Blocks until no readers and no other writer hold the lock.
 */
void rwlock_acquire_write(rwlock* lock);

/**
 * Releases the write lock.
 * Wakes a waiting writer if one exists; otherwise wakes all waiting readers.
 */
void rwlock_release_write(rwlock* lock);


#endif
