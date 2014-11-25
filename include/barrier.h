#ifndef BARRIER_H
#define BARRIER_H

#include <stdint.h>
#include "mutex.h"
#include "cond.h"

typedef struct {
    int32_t need;
    int32_t waiters;
    mutex_t mutex;
    cond_t cond;
    int32_t need_broadcast;
} barrier_t;

int32_t barrier_init(barrier_t *b, int32_t n);
int32_t barrier_destroy(barrier_t *b);
int32_t barrier_wait(barrier_t *b);
#endif
