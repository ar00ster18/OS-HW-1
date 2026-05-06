#ifndef TL_SEMAPHORE_H
#define TL_SEMAPHORE_H

/* =========================
   REQUIRED INCLUDES
   ========================= */

#include <stdatomic.h>


/* =========================
   TICKET LOCK
   ========================= */

// struct for ticket lock

typedef struct
{
    atomic_int ticket;
    atomic_int cur_ticket;

} ticket_lock;

// initializes the Ticket Lock
void ticketlock_init(ticket_lock* lock);

// Aquires the Ticket Lock (wait for turn)
void ticketlock_acquire(ticket_lock* lock);

// Releases the Ticket Lock
void ticketlock_release(ticket_lock* lock);



/* =========================
   SEMAPHORE
   ========================= */

// struct for  an unbounded semaphore protected by a ticket lock

typedef struct
{

    int value;
    ticket_lock lock;

} semaphore;

// Initialized the semaphore with an initial value
void semaphore_init(semaphore* sem, int initial_value);

// Decrements the semaphore aka waits for resource
void semaphore_wait(semaphore* sem);

// Increments semaphore aka signals resource is avalible
void semaphore_signal(semaphore* sem);


#endif