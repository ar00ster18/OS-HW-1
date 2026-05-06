#include "tl_semaphore.h"
#include <sched.h>

/* =========================
   TICKET LOCK
   ========================= */

/* Initializes the ticket lock by setting both ticket and cur_ticket to 0. */
void ticketlock_init(ticket_lock* lock)
{
    atomic_init(&lock->ticket, 0);
    atomic_init(&lock->cur_ticket, 0);
}

/* Acquires the ticket lock.
 * Atomically grabs a ticket number, then busy-waits (yielding) until
 * cur_ticket reaches our ticket, guaranteeing FIFO acquisition order. */
void ticketlock_acquire(ticket_lock*lock)
{
    // get my ticket
    int my_ticket = atomic_fetch_add(&lock->ticket, 1);

    // wait until it is my turn
    while(atomic_load(&lock->cur_ticket) !=my_ticket)
    {
    sched_yield();
    }
}

/* Releases the ticket lock by incrementing cur_ticket,
 * allowing the next waiting thread to proceed. */
void ticketlock_release(ticket_lock*lock)
{
    atomic_fetch_add(&lock->cur_ticket, 1);
}


/* =========================
   SEMAPHORE
   ========================= */

/* Initializes the semaphore with the given initial value (>= 0)
 * and initializes its internal ticket lock. */
void semaphore_init(semaphore* sem, int initial_value)
{
    // TODO
}

/* Decrements the semaphore value (P / down / wait).
 * If value > 0, decrements and returns immediately.
 * If value == 0, busy-waits (calling sched_yield) until value > 0,
 * then decrements. The internal ticket lock protects value access. */
void semaphore_wait(semaphore* sem)
{
    // TODO
}

/* Increments the semaphore value (V / up / signal).
 * Acquires the ticket lock, increments value, then releases the lock,
 * allowing any waiting thread in semaphore_wait to eventually proceed. */
void semaphore_signal(semaphore* sem)
{
    // TODO
}
