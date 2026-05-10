#ifndef COND_VAR_H
#define COND_VAR_H

#include <stdatomic.h>
#include "tl_semaphore.h"


/* =========================
   CONDITION VARIABLE
   ========================= */

/*
 * Condition variable backed by per-thread semaphores.
 *
 * Each waiting thread allocates its own semaphore on the stack and registers
 * a pointer to it here. Signal/broadcast operate on specific semaphore
 * pointers, preventing signal leakage across wait cycles.
 *
 * Correctness rules:
 *   - Signals are NOT remembered: signal/broadcast when no threads are
 *     waiting has no effect.
 *   - No lost notifications: a thread that has started waiting will
 *     eventually be woken by a subsequent signal/broadcast.
 */
#define CV_MAX_WAITERS 64

typedef struct
{
    semaphore* waiters[CV_MAX_WAITERS]; /* pointers to per-thread semaphores */
    atomic_int num_waiters;             /* number of threads currently waiting */
    ticket_lock internal_lock;          /* protects waiters[] and num_waiters */

} condition_variable;


/**
 * Initializes the condition variable.
 */
void condition_variable_init(condition_variable* cv);

/**
 * Causes the calling thread to wait on 'cv'.
 *
 * Precondition: the caller holds 'ext_lock'.
 * The function releases 'ext_lock' while waiting and reacquires it
 * before returning, so the caller always holds the lock on return.
 */
void condition_variable_wait(condition_variable* cv, ticket_lock* ext_lock);

/**
 * Wakes up exactly one thread currently waiting on 'cv'.
 * If no threads are waiting, does nothing.
 */
void condition_variable_signal(condition_variable* cv);

/**
 * Wakes up all threads currently waiting on 'cv'.
 * If no threads are waiting, does nothing.
 */
void condition_variable_broadcast(condition_variable* cv);


#endif
