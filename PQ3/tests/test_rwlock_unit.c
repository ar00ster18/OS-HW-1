#include <stdio.h>
#include <stdlib.h>

#include "../rw_lock.h"


int main(void)
{
    rwlock lock;

    /* initialize rw lock */
    rwlock_init(&lock);


    /* =========================
       READ LOCK TEST
       ========================= */

    rwlock_acquire_read(&lock);

    /*
        After acquiring read lock:
        - active readers should be 1
        - no active writer
    */
    if (lock.active_readers != 1 ||
        lock.active_writer != 0)
    {
        printf("FAIL: read lock acquire failed\n");
        return 1;
    }

    rwlock_release_read(&lock);

    /*
        After releasing:
        - no active readers
    */
    if (lock.active_readers != 0)
    {
        printf("FAIL: read lock release failed\n");
        return 1;
    }


    /* =========================
       WRITE LOCK TEST
       ========================= */

    rwlock_acquire_write(&lock);

    /*
        After acquiring write lock:
        - active writer should be 1
        - no active readers
    */
    if (lock.active_writer != 1 ||
        lock.active_readers != 0)
    {
        printf("FAIL: write lock acquire failed\n");
        return 1;
    }

    rwlock_release_write(&lock);

    /*
        After releasing:
        - no active writer
    */
    if (lock.active_writer != 0)
    {
        printf("FAIL: write lock release failed\n");
        return 1;
    }


    printf("PASS: rwlock unit test successful\n");

    return 0;
}