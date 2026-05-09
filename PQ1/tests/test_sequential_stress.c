#include <stdio.h>
#include "tl_semaphore.h"

/* Single-threaded stress test: verifies signal and wait are perfectly balanced
 * over many iterations with no concurrency involved. */

#define ITERATIONS 10000

int main(void)
{
    semaphore sem;
    semaphore_init(&sem, 0);

    for (int i = 0; i < ITERATIONS; i++)
    {
        semaphore_signal(&sem);
        semaphore_wait(&sem);
    }

    if (sem.value != 0)
    {
        fprintf(stderr, "FAIL: expected final value 0, got %d\n", sem.value);
        return 1;
    }

    printf("PASS: %d signal/wait pairs, final value == 0\n", ITERATIONS);
    return 0;
}
