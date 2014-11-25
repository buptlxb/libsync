#include "barrier.h"

int32_t barrier_init(barrier_t *b, int32_t n)
{
    b->need = n;
    b->waiters = 0;
    mutex_init(&b->mutex);
    cond_init(&b->cond);
    b->need_broadcast = 1;
    return 0;
}
int32_t barrier_destroy(barrier_t *b)
{
    mutex_destroy(&b->mutex);
    cond_destroy(&b->cond);
    return 0;
}

int32_t barrier_wait(barrier_t *b)
{
    mutex_lock(&b->mutex);
    b->waiters++;
    while (b->waiters != b->need) {
        cond_wait(&b->cond, &b->mutex);
    }
    if (b->need_broadcast) {
        b->need_broadcast = 0;
        mutex_unlock(&b->mutex);
        cond_broadcast(&b->cond);
    } else {
        mutex_unlock(&b->mutex);
    }
    return 0;
}
