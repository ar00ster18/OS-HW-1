#ifndef RW_LOCK_H
#define RW_LOCK_H

/* =========================
   REQUIRED INCLUDES FROM PQ1 & PQ2
   ========================= */

#include "tl_semaphore.h"
#include "cond_var.h"


/* =========================
   READER WRITER LOCK
   ========================= */

/*
    Writer-preference reader-writer lock.

    - Multiple readers may enter simultaneously
    - Writers require exclusive access
    - Waiting writers block new readers
*/
typedef struct
{
    /* protects all internal state */
    ticket_lock lock;

    /* condition variable for waiting readers */
    condition_variable readers_cv;

    /* condition variable for waiting writers */
    condition_variable writers_cv;

    /* number of readers currently holding lock */
    int active_readers;

    /* number of writers waiting to acquire */
    int waiting_writers;

    /* whether writer currently holds lock (0/1) */
    int active_writer;

} rwlock;


/* initializes the read-write lock */
void rwlock_init(rwlock* lock);


/* acquires lock for reading */
void rwlock_acquire_read(rwlock* lock);


/* releases read lock */
void rwlock_release_read(rwlock* lock);


/* acquires lock for writing (exclusive access) */
void rwlock_acquire_write(rwlock* lock);


/* releases write lock */
void rwlock_release_write(rwlock* lock);


#endif