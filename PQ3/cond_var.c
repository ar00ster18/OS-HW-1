#include "cond_var.h"

/* Initializes the condition variable. */
void condition_variable_init(condition_variable* cv)
{
    atomic_init(&cv->num_waiters, 0);
    ticketlock_init(&cv->internal_lock);
}

/* Causes the calling thread to wait on 'cv'.
 *
 * Each thread allocates its own semaphore on the stack and registers a pointer
 * to it in cv->waiters[]. Signal/broadcast operate on specific semaphore
 * pointers, so signals from one broadcast cannot leak into the next wait cycle.
 *
 * Timeline:
 *   1. Create a fresh semaphore (value 0) on the stack
 *   2. Acquire internal_lock, push pointer into waiters[], release ext_lock
 *   3. Release internal_lock
 *   4. Block on own semaphore
 *   5. Re-acquire ext_lock before returning
 */
void condition_variable_wait(condition_variable* cv, ticket_lock* ext_lock)
{
    semaphore my_sem;
    semaphore_init(&my_sem, 0);

    ticketlock_acquire(&cv->internal_lock);
    int idx = atomic_fetch_add(&cv->num_waiters, 1);
    cv->waiters[idx] = &my_sem;
    ticketlock_release(ext_lock);
    ticketlock_release(&cv->internal_lock);

    semaphore_wait(&my_sem);

    ticketlock_acquire(ext_lock);
}

/* Wakes up exactly one waiting thread (if any).
 * Pops the last registered semaphore and signals it directly. */
void condition_variable_signal(condition_variable* cv)
{
    ticketlock_acquire(&cv->internal_lock);
    if (atomic_load(&cv->num_waiters) > 0)
    {
        int idx = atomic_fetch_sub(&cv->num_waiters, 1) - 1;
        semaphore* sem = cv->waiters[idx];
        ticketlock_release(&cv->internal_lock);
        semaphore_signal(sem);
    }
    else
    {
        ticketlock_release(&cv->internal_lock);
    }
}

/* Wakes up all waiting threads.
 * Copies all semaphore pointers locally, resets num_waiters to 0,
 * then signals each thread's semaphore outside the lock. */
void condition_variable_broadcast(condition_variable* cv)
{
    semaphore* sems[CV_MAX_WAITERS];

    ticketlock_acquire(&cv->internal_lock);
    int n = atomic_load(&cv->num_waiters);
    for (int i = 0; i < n; i++)
    {
        sems[i] = cv->waiters[i];
    }
    atomic_store(&cv->num_waiters, 0);
    ticketlock_release(&cv->internal_lock);

    for (int i = 0; i < n; i++)
    {
        semaphore_signal(sems[i]);
    }
}
