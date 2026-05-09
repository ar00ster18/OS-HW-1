#include "cond_var.h"
#include <sched.h>

/* Initializes the condition variable.
 * Sets up the internal semaphore for waiting threads. */
void condition_variable_init(condition_variable* cv)
{
    // TODO
}

/* Causes the calling thread to wait on 'cv'.
 * Registers the thread as waiting, releases ext_lock, busy-waits until
 * woken by signal/broadcast, then reacquires ext_lock before returning. */
void condition_variable_wait(condition_variable* cv, ticket_lock* ext_lock)
{
    // TODO
}

/* Wakes up exactly one thread currently waiting on 'cv'.
 * If no threads are waiting, does nothing. */
void condition_variable_signal(condition_variable* cv)
{
    // TODO
}

/* Wakes up all threads currently waiting on 'cv'.
 * If no threads are waiting, does nothing. */
void condition_variable_broadcast(condition_variable* cv)
{
    // TODO
}
