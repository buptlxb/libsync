#include "cond.h"
#include "atomic.h"
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdlib.h>
#include <limits.h>


int32_t cond_init(cond_t *cond)
{
    cond->seq = 0;
    cond->lock = NULL;
    return 0;
}

int32_t cond_destroy(cond_t *cond)
{
    return 0;
}

int32_t cond_wait(cond_t *cond, mutex_t *lock)
{
    DEBUG(fprintf(stderr, "[%d] cond_wait start\n", pthread_self()));
    int32_t seq = cond->seq;

    if (cond->lock != lock)
    {
        if (cond->lock) return EINVAL;
        
        /* Atomically set mutex inside cv */
        atomic_cmpxchg_64((int64_t *)&cond->lock, (int64_t )NULL, (int64_t)lock);
        if (cond->lock != lock) return EINVAL;
    }
    
    mutex_unlock(lock);
    
    int32_t ret = syscall(SYS_futex, &cond->seq, FUTEX_WAIT_PRIVATE, seq, NULL, NULL, 0);
    DEBUG(fprintf(stderr, "[%d] syscall(SYS_futex, 0x%p(%d), FUTEX_WAKE_PRIVATE, %d, %p, %p, %d)=%d\n",
                    pthread_self(), &cond->seq, cond->seq, seq, NULL, NULL, 0, ret));
    
    while (atomic_xchg_32(lock, LOCKED_WITH_WAITERS))
    {
        int32_t ret = syscall(SYS_futex, lock, FUTEX_WAIT_PRIVATE, LOCKED_WITH_WAITERS, NULL, NULL, 0);
        DEBUG(fprintf(stderr, "[%d] syscall(SYS_futex, 0x%p(%d), FUTEX_WAKE_PRIVATE, %d, %p, %p, %d)=%d\n",
                    pthread_self(), lock, *lock, LOCKED_WITH_WAITERS, NULL, NULL, 0, ret));
    }

    DEBUG(fprintf(stderr, "[%d] cond_wait end\n", pthread_self()));
        
    return 0;
}

int32_t cond_signal(cond_t *cond)
{
    DEBUG(fprintf(stderr, "[%d] cond_signal start\n", pthread_self()));
    /* We are waking someone up */
    atomic_add_32(&cond->seq, 1);
    
    /* Wake up a thread */
    int32_t ret = syscall(SYS_futex, &cond->seq, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
    DEBUG(fprintf(stderr, "[%d] syscall(SYS_futex, 0x%p(%d), FUTEX_WAKE_PRIVATE, %d, %p, %p, %d)=%d\n",
                    pthread_self(), &cond->seq, cond->seq, 1, NULL, NULL, 0, ret));
    
    DEBUG(fprintf(stderr, "[%d] cond_signal end\n", pthread_self()));
    return 0;
}

int32_t cond_broadcast(cond_t *cond)
{
    DEBUG(fprintf(stderr, "[%d] cond_broadcast start\n", pthread_self()));
    mutex_t *lock = cond->lock;
    
    /* No mutex means that there are no waiters */
    if (!lock) return 0;
    
    /* We are waking everyone up */
    atomic_add_32(&cond->seq, 1);
    
    /* Wake one thread, and requeue the rest on the mutex */
    int32_t ret = syscall(SYS_futex, &cond->seq, FUTEX_REQUEUE_PRIVATE, 1, (void *) INT_MAX, lock, 0);
    DEBUG(fprintf(stderr, "[%d] syscall(SYS_futex, 0x%p(%d), FUTEX_REQUEUE_PRIVATE, %d, %p, %p, %d)=%d\n",
                    pthread_self(), &cond->seq, cond->seq, 1, (void *) INT_MAX, lock, 0, ret));
    
    DEBUG(fprintf(stderr, "[%d] cond_broadcast end\n", pthread_self()));
    return 0;
}
