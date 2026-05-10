#include "rw_lock.h"


/* =========================
   READERS-WRITER LOCK
   ========================= */

void rwlock_init(rwlock* lock)
{
    ticketlock_init(&lock->lock);
    condition_variable_init(&lock->readers_cv);
    condition_variable_init(&lock->writers_cv);
    lock->active_readers  = 0;
    lock->waiting_writers = 0;
    lock->active_writer   = 0;
}


void rwlock_acquire_read(rwlock* lock)
{
    ticketlock_acquire(&lock->lock);

    /* Writer preference: block new readers while any writer holds or is waiting. */
    while (lock->active_writer == 1 || lock->waiting_writers > 0)
    {
        condition_variable_wait(&lock->readers_cv, &lock->lock);
    }

    lock->active_readers++;

    ticketlock_release(&lock->lock);
}


void rwlock_release_read(rwlock* lock)
{
    ticketlock_acquire(&lock->lock);

    lock->active_readers--;

    /* If we were the last active reader, a waiting writer can now proceed. */
    if (lock->active_readers == 0 && lock->waiting_writers > 0)
    {
        condition_variable_signal(&lock->writers_cv);
    }

    ticketlock_release(&lock->lock);
}


void rwlock_acquire_write(rwlock* lock)
{
    ticketlock_acquire(&lock->lock);

    lock->waiting_writers++;

    /* Wait until no readers and no other writer are active. */
    while (lock->active_writer == 1 || lock->active_readers > 0)
    {
        condition_variable_wait(&lock->writers_cv, &lock->lock);
    }

    lock->active_writer = 1;
    lock->waiting_writers--;

    ticketlock_release(&lock->lock);
}


void rwlock_release_write(rwlock* lock)
{
    ticketlock_acquire(&lock->lock);

    lock->active_writer = 0;

    /*
     * Writer preference: if writers are waiting, wake one of them.
     * Otherwise, wake all waiting readers so they can enter concurrently.
     */
    if (lock->waiting_writers > 0)
    {
        condition_variable_signal(&lock->writers_cv);
    }
    else
    {
        condition_variable_broadcast(&lock->readers_cv);
    }

    ticketlock_release(&lock->lock);
}
