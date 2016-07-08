#include <stdio.h>
#include <pthread.h>
#include "cond.h"
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>

typedef struct work {
    struct work *next;
    int32_t val;
} work_t;

typedef struct {
    work_t *head;
    work_t *tail;
} work_queue_t;

cond_t cond;
mutex_t lock;

work_queue_t workq;
int32_t counter = 0;
volatile int32_t end = 1;

int is_empty(work_queue_t *wq)
{
    return wq->head == NULL;
}

int32_t dequeue(work_queue_t *wq)
{
    if (wq->head == NULL) {
        perror("dequeue");
        exit(-1);
    }
    work_t *tmp = wq->head;
    int32_t ret = tmp->val;
    wq->head = wq->head->next;
    if (wq->head == NULL) {
        wq->tail = NULL;
    }
    free(tmp);
    return ret;
}

void enqueue(work_queue_t *wq, int32_t val)
{
    work_t *tmp = malloc(sizeof(work_t));
    tmp->val = val;
    tmp->next = NULL;

    if (wq->head == NULL) {
        wq->head = wq->tail = tmp;
    } else {
        wq->tail->next = tmp;
        wq->tail = tmp;
    }
}

void *worker(void *arg)
{
    while (end) {
        printf("worker\n");
        mutex_lock(&lock);
        while (is_empty(&workq)) {
            if (EINVAL == cond_wait(&cond, &lock)) {
                printf("cond_wait\n");
                exit(-1);
            }
            if (!end) {
                mutex_unlock(&lock);
                printf("worker exit\n");
                return NULL;
            }
        }
        int32_t val = dequeue(&workq);
        mutex_unlock(&lock);
        counter += val;
        printf("counter=%d\n", counter);
    }
    return NULL;
}

int32_t main(void)
{
    mutex_init(&lock);
    cond_init(&cond);
    workq.head = workq.tail = NULL;

    pthread_t tid[10];
    int32_t i;
    for (i = 0; i < 10; i++) {
        int32_t ret = pthread_create(tid+i, NULL, worker, NULL);
//        printf("%x\n", tid[i]);
        if (ret) {
            perror("pthread_create");
            return -1;
        }
    }

    for (i = 0; i < 100; i++) {
        enqueue(&workq, i); 
        cond_signal(&cond);
        printf("work[%d]\n", i);
    }

    sleep(2);
    printf("finish\n");
    if (is_empty(&workq))
        end = 0;

    printf("ready\n");
    cond_broadcast(&cond);
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
