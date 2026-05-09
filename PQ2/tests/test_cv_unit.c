#include <stdio.h>
#include "cond_var.h"

/* Unit test (single thread): verifies basic init behavior and that
 * signal/broadcast with no waiters does not crash or hang. */

int main(void)
{
    condition_variable cv;
    ticket_lock lock;

    condition_variable_init(&cv);
    ticketlock_init(&lock);

    /* signal with no waiters should do nothing */
    condition_variable_signal(&cv);
    condition_variable_signal(&cv);
    printf("PASS: signal with no waiters does not crash\n");

    /* broadcast with no waiters should do nothing */
    condition_variable_broadcast(&cv);
    condition_variable_broadcast(&cv);
    printf("PASS: broadcast with no waiters does not crash\n");

    /* num_waiters should still be 0 */
    if (cv.num_waiters != 0)
    {
        fprintf(stderr, "FAIL: expected num_waiters == 0, got %d\n", cv.num_waiters);
        return 1;
    }
    printf("PASS: num_waiters is 0 after spurious signals\n");

    printf("All unit tests passed.\n");
    return 0;
}
