#include "mutex.h"
#include "atomic.h"
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdlib.h>

int32_t mutex_init(mutex_t *m)
{
    *m = MUTEX_INITIALIZER;
    return 0;
}

int32_t mutex_destroy(mutex_t *m)
{
    return 0;
}

int32_t mutex_lock(mutex_t *m)
{
    DEBUG(fprintf(stderr, "[%d] mutex_lock start\n", pthread_self()));
    int32_t i, c;
    
    /* Spin and try to take lock */
#ifdef SPIN
    for (i = 0; i < SPIN_TIMES; i++)
    {
#endif
        c = atomic_cmpxchg_32(m, UNLOCK, LOCKED);
        if (!c) {
            DEBUG(fprintf(stderr, "[%d] mutex_lock end\n", pthread_self()));
            return 0;
        }
        
        cpu_relax();
#ifdef SPIN
    }
#endif

    /* The lock is now contended */
    if (c == LOCKED) c = xchg_32(m, LOCKED_WITH_WAITERS);

    while (c)
    {
        /* Wait in the kernel */
        int32_t ret = syscall(SYS_futex, m, FUTEX_WAIT_PRIVATE, LOCKED_WITH_WAITERS, NULL, NULL, 0);
        DEBUG(fprintf(stderr, "[%d] syscall(SYS_futex, 0x%p, FUTEX_WAIT_PRIVATE, %d, %p, %p, %d)=%d\n",
                    pthread_self(), m, LOCKED_WITH_WAITERS, NULL, NULL, 0, ret));
        c = xchg_32(m, LOCKED_WITH_WAITERS);
    }
    
    DEBUG(fprintf(stderr, "[%d] mutex_lock end\n", pthread_self()));
    return 0;
}

int32_t mutex_unlock(mutex_t *m)
{
    DEBUG(fprintf(stderr, "[%d] mutex_unlock start\n", pthread_self()));
    int32_t i;
    
    /* Unlock, and if not contended then exit. */
    if (*m == LOCKED_WITH_WAITERS)
    {
        *m = UNLOCK;
    }
    else if (xchg_32(m, UNLOCK) == LOCKED) {
        DEBUG(fprintf(stderr, "[%d] mutex_unlock end\n", pthread_self()));
        return 0;
    }

    /* Spin and hope someone takes the lock */
#ifdef SPIN
    for (i = 0; i < SPIN_TIMES; i++)
    {
#endif
        if (*m)
        {
            /* Need to set to state 2 because there may be waiters */
            if (atomic_cmpxchg_32(m, LOCKED, LOCKED_WITH_WAITERS)) {
                DEBUG(fprintf(stderr, "[%d] mutex_unlock end\n", pthread_self()));
                return 0;
            }
        }
        cpu_relax();
#ifdef SPIN
    }
#endif
    
    /* We need to wake someone up */
    int32_t ret = syscall(SYS_futex, m, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
    DEBUG(fprintf(stderr, "[%d] syscall(SYS_futex, 0x%p, FUTEX_WAKE_PRIVATE, %d, %p, %p, %d)=%d\n",
                    pthread_self(), m, 1, NULL, NULL, 0, ret));
    
    DEBUG(fprintf(stderr, "[%d] mutex_unlock end\n", pthread_self()));
    return 0;
}

int32_t mutex_trylock(mutex_t *m)
{
    /* Try to take the lock, if is currently unlocked */
    uint32_t c = atomic_cmpxchg_32(m, UNLOCK, LOCKED);
    if (!c) return 0;
    return EBUSY;
}
