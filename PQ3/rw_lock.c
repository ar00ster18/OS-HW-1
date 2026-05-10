#include "rw_lock.h"


/* =========================
   READER WRITER LOCK
   ========================= */

void rwlock_init(rwlock* lock)
{
    /*Initialized the lock*/
    ticketlock_init(&lock->lock);

    /*Initialized the readers cond variable*/
    condition_variable_init(&lock->readers_cv);

    /*Initialized the writers cond variable*/
    condition_variable_init(&lock->writers_cv);

    /*Initially there are no active readers*/
    lock->active_readers = 0;

    /*Initially there are no writing writers*/
    lock->waiting_writers = 0;

    /*Initially there are no active writers*/
    lock->active_writer = 0;
}


void rwlock_acquire_read(rwlock* lock)
{
    /*
        IMPLEMENT HERE
    */
}


void rwlock_release_read(rwlock* lock)
{
    /*
        IMPLEMENT HERE
    */
}


void rwlock_acquire_write(rwlock* lock)
{
    /*
        IMPLEMENT HERE
    */
}


void rwlock_release_write(rwlock* lock)
{
    /*
        IMPLEMENT HERE
    */
}