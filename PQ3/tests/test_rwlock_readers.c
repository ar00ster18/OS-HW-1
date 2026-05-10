#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../rw_lock.h"


#define NUM_READERS 8


rwlock lock;

/* current readers inside critical section */
int current_readers = 0;

/* maximum simultaneous readers observed */
int max_readers = 0;


/* reader thread */
void* reader_thread(void* arg)
{
    (void)arg;

    rwlock_acquire_read(&lock);

    /* reader entered */
    current_readers++;

    /* update max readers seen */
    if (current_readers > max_readers)
    {
        max_readers = current_readers;
    }

    /* force overlap between readers */
    usleep(100000);

    /* reader leaving */
    current_readers--;

    rwlock_release_read(&lock);

    return NULL;
}


int main(void)
{
    pthread_t threads[NUM_READERS];

    /* initialize rw lock */
    rwlock_init(&lock);

    /* create reader threads */
    for (int i = 0; i < NUM_READERS; i++)
    {
        if (pthread_create(&threads[i],
                           NULL,
                           reader_thread,
                           NULL) != 0)
        {
            perror("pthread_create");
            return 1;
        }
    }

    /* wait for all readers */
    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    /*
        If multiple readers entered simultaneously,
        max_readers should be > 1
    */
    if (max_readers > 1)
    {
        printf("PASS: multiple readers entered simultaneously\n");
        return 0;
    }
    else
    {
        printf("FAIL: readers did not overlap\n");
        return 1;
    }
}