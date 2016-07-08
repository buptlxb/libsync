#include <stdio.h>
#include <pthread.h>
#include "mutex.h"
#include <stdint.h>

mutex_t lock;

int32_t counter = 0;

void *thread(void *arg)
{
    //printf("child\n");

    int32_t i;
    for (i = 0; i < 1000; i++) {
        mutex_lock(&lock);
        //printf("child lock\n");
        counter++;
        //printf("child unlock\n");
        mutex_unlock(&lock);
    }
}

int32_t main(void)
{
    mutex_init(&lock);

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

    for (i = 0; i < 10; i++) {
        pthread_join(tid[i], NULL);
    }

    //printf("parent unlock\n");
    //mutex_unlock(&lock);

    //sleep(2);
    //printf("mutex_t destroy\n");
    mutex_destroy(&lock);
    printf("counter = %d\n", counter);
    return 0;
}
