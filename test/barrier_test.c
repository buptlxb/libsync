#include <stdio.h>
#include <pthread.h>
#include "barrier.h"
#include <stdint.h>

barrier_t b;

int32_t counter = 0;

void *thread(void *arg)
{
    barrier_wait(&b);
    printf("child\n");

    int32_t i;
    for (i = 0; i < 1000; i++) {
        //printf("child lock\n");
        counter++;
        //printf("child unlock\n");
    }
}

int32_t main(void)
{
    barrier_init(&b, 11);

    printf("parent\n");

    //mutex_lock(&lock);
    //printf("parent lock\n");

    pthread_t tid[10];
    int32_t i;
    for (i = 0; i < 10; i++) {
        int32_t ret = pthread_create(tid+i, NULL, thread, NULL);
//        printf("%x\n", tid[i]);
        if (ret) {
            perror("pthread_create");
            return -1;
        }
    }

    sleep(4);
    barrier_wait(&b);

    for (i = 0; i < 10; i++) {
        pthread_join(tid[i], NULL);
    }

    //printf("parent unlock\n");
    //mutex_unlock(&lock);

    //sleep(2);
    //printf("mutex_t destroy\n");
    barrier_destroy(&b);
    printf("counter = %d\n", counter);
    return 0;
}
