#ifndef COND_H
#define COND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mutex.h"

typedef struct {
    mutex_t *lock;
    int32_t seq;
    int32_t pad;
} cond_t;

#define COND_INITIALIZER {NULL, 0, 0}

int32_t cond_init(cond_t *cond);
int32_t cond_destroy(cond_t *cond);
int32_t cond_wait(cond_t *cond, mutex_t *lock);
int32_t cond_signal(cond_t *cond);
int32_t cond_broadcast(cond_t *cond);

#ifdef __cplusplus
}
#endif

#endif
