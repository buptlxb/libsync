#include "mutex.h"
#include "atomic.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdlib.h>

#define EBUSY -1

int mutex_init(mutex *m)
{
    *m = 0;
    return 0;
}

int mutex_destroy(mutex *m)
{
    return 0;
}

int mutex_lock(mutex *m)
{
    DEBUG(fprintf(stderr, "[%d] mutex_lock start\n", pthread_self()));
    int i, c;
    
    /* Spin and try to take lock */
    for (i = 0; i < 100; i++)
    {
        c = atomic_cmpxchg(m, 0, 1);
        if (!c) {
            DEBUG(fprintf(stderr, "[%d] mutex_lock end\n", pthread_self()));
            return 0;
        }
        
        cpu_relax();
    }

    /* The lock is now contended */
    if (c == 1) c = xchg_32(m, 2);

    while (c)
    {
        /* Wait in the kernel */
        int ret = syscall(SYS_futex, m, FUTEX_WAIT_PRIVATE, 2, NULL, NULL, 0);
        DEBUG(fprintf(stderr, "[%d] syscall(SYS_futex, 0x%p, FUTEX_WAIT_PRIVATE, %d, %p, %p, %d)=%d\n",
                    pthread_self(), m, 2, NULL, NULL, 0, ret));
        c = xchg_32(m, 2);
    }
    
    DEBUG(fprintf(stderr, "[%d] mutex_lock end\n", pthread_self()));
    return 0;
}

int mutex_unlock(mutex *m)
{
    DEBUG(fprintf(stderr, "[%d] mutex_unlock start\n", pthread_self()));
    int i;
    
    /* Unlock, and if not contended then exit. */
    if (*m == 2)
    {
        *m = 0;
    }
    else if (xchg_32(m, 0) == 1) {
        DEBUG(fprintf(stderr, "[%d] mutex_unlock end\n", pthread_self()));
        return 0;
    }

    /* Spin and hope someone takes the lock */
    for (i = 0; i < 200; i++)
    {
        if (*m)
        {
            /* Need to set to state 2 because there may be waiters */
            if (atomic_cmpxchg(m, 1, 2)) {
                DEBUG(fprintf(stderr, "[%d] mutex_unlock end\n", pthread_self()));
                return 0;
            }
        }
        cpu_relax();
    }
    
    /* We need to wake someone up */
    int ret = syscall(SYS_futex, m, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
    DEBUG(fprintf(stderr, "[%d] syscall(SYS_futex, 0x%p, FUTEX_WAKE_PRIVATE, %d, %p, %p, %d)=%d\n",
                    pthread_self(), m, 1, NULL, NULL, 0, ret));
    
    DEBUG(fprintf(stderr, "[%d] mutex_unlock end\n", pthread_self()));
    return 0;
}

int mutex_trylock(mutex *m)
{
    /* Try to take the lock, if is currently unlocked */
    unsigned c = atomic_cmpxchg(m, 0, 1);
    if (!c) return 0;
    return EBUSY;
}
