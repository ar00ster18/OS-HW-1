#ifndef TL_SEMAPHORE_H
#define TL_SEMAPHORE_H

/* =========================
   REQUIRED INCLUDES
   ========================= */

#include <stdatomic.h>


/* =========================
   TICKET LOCK
   ========================= */

/*
    IMPLEMENT:
    - atomic ticket counter
    - atomic current ticket
*/
typedef struct
{
    atomic_int ticket;
    atomic_int cur_ticket;

} ticket_lock;


/*
    IMPLEMENT IN tl_semaphore.c:

    initialize:
    - ticket
    - cur_ticket
*/
void ticketlock_init(ticket_lock* lock);


/*
    IMPLEMENT IN tl_semaphore.c:

    1. take a ticket using atomic_fetch_add
    2. wait until current ticket == my ticket
    3. while waiting:
            sched_yield()
*/
void ticketlock_acquire(ticket_lock* lock);


/*
    IMPLEMENT IN tl_semaphore.c:

    increment current ticket
*/
void ticketlock_release(ticket_lock* lock);



/* =========================
   SEMAPHORE
   ========================= */

/*
    IMPLEMENT:
    - integer value
    - internal ticket lock
*/
typedef struct
{




} semaphore;


/*
    IMPLEMENT IN tl_semaphore.c:

    initialize:
    - semaphore value
    - internal ticket lock
*/
void semaphore_init(semaphore* sem, int initial_value);


/*
    IMPLEMENT IN tl_semaphore.c:

    LOOP FOREVER:

        acquire internal ticket lock

        if value > 0:
            decrement value
            release lock
            return

        release lock

        sched_yield()
*/
void semaphore_wait(semaphore* sem);


/*
    IMPLEMENT IN tl_semaphore.c:

    acquire internal lock

    increment value

    release lock
*/
void semaphore_signal(semaphore* sem);


#endif