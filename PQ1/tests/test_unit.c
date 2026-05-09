#include <stdio.h>
#include "tl_semaphore.h"

/* Single-threaded unit tests for basic semaphore behavior. */

static int failed = 0;

static void check(const char* desc, int condition)
{
    if (condition)
    {
        printf("PASS: %s\n", desc);
    }
    else
    {
        fprintf(stderr, "FAIL: %s\n", desc);
        failed = 1;
    }
}

int main(void)
{
    semaphore sem;

    /* init(1) -> wait -> value should be 0 */
    semaphore_init(&sem, 1);
    semaphore_wait(&sem);
    check("init(1) -> wait -> value == 0", sem.value == 0);

    /* signal -> value should be 1 */
    semaphore_signal(&sem);
    check("signal -> value == 1", sem.value == 1);

    /* init(3) -> wait x3 -> value should be 0 */
    semaphore_init(&sem, 3);
    semaphore_wait(&sem);
    semaphore_wait(&sem);
    semaphore_wait(&sem);
    check("init(3) -> wait x3 -> value == 0", sem.value == 0);

    /* init(0) -> signal -> wait -> value should be 0 */
    semaphore_init(&sem, 0);
    semaphore_signal(&sem);
    semaphore_wait(&sem);
    check("init(0) -> signal -> wait -> value == 0", sem.value == 0);

    /* multiple signals then multiple waits */
    semaphore_init(&sem, 0);
    semaphore_signal(&sem);
    semaphore_signal(&sem);
    semaphore_signal(&sem);
    semaphore_wait(&sem);
    semaphore_wait(&sem);
    semaphore_wait(&sem);
    check("init(0) -> signal x3 -> wait x3 -> value == 0", sem.value == 0);

    return failed;
}
