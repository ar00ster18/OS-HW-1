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

    /*Initially there are no waiting writers*/
    lock->waiting_writers = 0;

    /*Initially there are no active writers*/
    lock->active_writer = 0;
}


void rwlock_acquire_read(rwlock* lock)
{
        /* protect internal state */
    ticketlock_acquire(&lock->lock);

    /*
        Readers must wait if:
        - a writer currently holds the lock
        - writers are waiting (writer preference)
    */
    while (lock->active_writer == 1 ||
           lock->waiting_writers > 0)
    {
        condition_variable_wait(&lock->readers_cv,
                                &lock->lock);
    }

    /* reader enters */
    lock->active_readers++;

    /* release internal state lock */
    ticketlock_release(&lock->lock);

}


void rwlock_release_read(rwlock* lock)
{
   /* protect internal state */
    ticketlock_acquire(&lock->lock);

    /* reader leaves */
    lock->active_readers--;

    /*
        If this was the last reader
        and writers are waiting,
        wake one writer
    */
    if (lock->active_readers == 0 &&
        lock->waiting_writers > 0)
    {
        condition_variable_signal(&lock->writers_cv);
    }

    /* release internal state lock */
    ticketlock_release(&lock->lock);
}


void rwlock_acquire_write(rwlock* lock)
{
    /* protect internal state */
    ticketlock_acquire(&lock->lock);

    /* writer announces it is waiting */
    lock->waiting_writers++;

    /*
        Writers must wait if:
        - another writer is active
        - readers are active
    */
    while (lock->active_writer == 1 ||
           lock->active_readers > 0)
    {
        condition_variable_wait(&lock->writers_cv,
                                &lock->lock);
    }

        /* writer enters exclusively */
    lock->active_writer = 1;

        /* writer is no longer waiting */
    lock->waiting_writers--;

    /* release internal state lock */
    ticketlock_release(&lock->lock);
}


void rwlock_release_write(rwlock* lock)
{
    /* protect internal state */
    ticketlock_acquire(&lock->lock);

    /* writer leaves */
    lock->active_writer = 0;

    /*
        Writer preference:
        if writers are waiting,
        wake one writer.

        Otherwise wake all readers.
    */
    if (lock->waiting_writers > 0)
    {
        condition_variable_signal(&lock->writers_cv);
    }
    else
    {
        condition_variable_broadcast(&lock->readers_cv);
    }

    /* release internal state lock */
    ticketlock_release(&lock->lock);
}