#ifndef TL_SEMAPHORE_H
#define TL_SEMAPHORE_H

#include <stdatomic.h>


/* =========================
   TICKET LOCK
   ========================= */

/*
 * FIFO spinlock implemented with fetch-and-add.
 * Each thread atomically takes a ticket and waits until cur_ticket matches.
 */
typedef struct
{
    atomic_int ticket;
    atomic_int cur_ticket;
} ticket_lock;

/**
 * Initializes the ticket lock, setting both ticket and cur_ticket to 0.
 */
void ticketlock_init(ticket_lock* lock);

/**
 * Acquires the ticket lock.
 * Atomically takes a ticket, then busy-waits (yielding) until it is this
 * thread's turn, guaranteeing FIFO acquisition order.
 */
void ticketlock_acquire(ticket_lock* lock);

/**
 * Releases the ticket lock by advancing cur_ticket,
 * allowing the next waiting thread to proceed.
 */
void ticketlock_release(ticket_lock* lock);


/* =========================
   SEMAPHORE
   ========================= */

/*
 * Unbounded semaphore protected by a ticket lock.
 * value is not required to be atomic since it is always accessed under lock.
 */
typedef struct
{
    int value;
    ticket_lock lock;
} semaphore;

/**
 * Initializes the semaphore with the given initial value (>= 0).
 */
void semaphore_init(semaphore* sem, int initial_value);

/**
 * Decrements the semaphore (P / wait / down).
 * If value == 0, busy-waits (calling sched_yield) until value > 0, then decrements.
 */
void semaphore_wait(semaphore* sem);

/**
 * Increments the semaphore (V / signal / up).
 * Allows a thread blocked in semaphore_wait to eventually proceed.
 */
void semaphore_signal(semaphore* sem);


#endif
