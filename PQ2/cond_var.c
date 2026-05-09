#include "cond_var.h"

/* Initializes the condition variable.
 * num_waiters starts at 0, internal_lock is fresh, sem starts at 0
 * so the first semaphore_wait will block. */
void condition_variable_init(condition_variable* cv)
{
    cv->num_waiters = 0;
    ticketlock_init(&cv->internal_lock);
    semaphore_init(&cv->sem, 0);
}

/* Causes the calling thread to wait on 'cv'.
 *
 * Correctness relies on the caller holding ext_lock when calling this function.
 * We increment num_waiters while holding internal_lock BEFORE releasing ext_lock.
 * This ensures that any concurrent signal/broadcast that runs after we release
 * ext_lock will see num_waiters > 0 and will post to sem, so we won't miss it.
 *
 * Timeline:
 *   1. Acquire internal_lock
 *   2. Increment num_waiters
 *   3. Release ext_lock   <- signaler can now enter
 *   4. Release internal_lock  <- signaler can now see num_waiters > 0
 *   5. semaphore_wait     <- block until signaled
 *   6. Re-acquire ext_lock
 */
void condition_variable_wait(condition_variable* cv, ticket_lock* ext_lock)
{
    ticketlock_acquire(&cv->internal_lock);
    cv->num_waiters++;
    ticketlock_release(ext_lock);
    ticketlock_release(&cv->internal_lock);

    semaphore_wait(&cv->sem);

    ticketlock_acquire(ext_lock);
}

/* Wakes up exactly one waiting thread (if any).
 * If num_waiters == 0, does nothing (signals are not remembered). */
void condition_variable_signal(condition_variable* cv)
{
    ticketlock_acquire(&cv->internal_lock);
    if (cv->num_waiters > 0)
    {
        cv->num_waiters--;
        semaphore_signal(&cv->sem);
    }
    ticketlock_release(&cv->internal_lock);
}

/* Wakes up all waiting threads.
 * If num_waiters == 0, does nothing. */
void condition_variable_broadcast(condition_variable* cv)
{
    ticketlock_acquire(&cv->internal_lock);
    while (cv->num_waiters > 0)
    {
        cv->num_waiters--;
        semaphore_signal(&cv->sem);
    }
    ticketlock_release(&cv->internal_lock);
}
