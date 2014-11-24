#ifndef ATOMIC_H
#define ATOMIC_H

#ifdef _DEBUG

#include <stdio.h>
#include <pthread.h>

#define DEBUG(str) str

#else /* else _DEBUG */

#define DEBUG(str)

#endif /* end _DEBUG */

#include <sched.h>

static inline int atomic_cmpxchg(int *v, int o, int n)
{
    DEBUG(fprintf(stderr, "[%d] cmpxchg(0x%p(%d), %d, %d)\n", pthread_self(), v, *v, o, n));
    int tmp = 0;
    __asm__ __volatile__("lock cmpxchg %[n], %[v]\n\t"
            :"=a"(tmp)
            :"a"(o), [n]"r"(n), [v]"m"(*v)
            :"cc", "memory"
            );
    DEBUG(fprintf(stderr, "[%d] cmpxchg(0x%p(%d), %d, %d)=%d\n", pthread_self(), v, *v, o, n, tmp));
    return tmp;
}

static inline int xchg_32(int *a, int b)
{
    DEBUG(fprintf(stderr, "[%d] xchg(0x%p(%d), %d)\n", pthread_self(), a, *a, b));
    __asm__ __volatile__("xchg %[b], %[a]\n\t"
            :[b]"+r"(b):[a]"m"(*a)
            :"memory"
            );
    DEBUG(fprintf(stderr, "[%d] xchg(0x%p(%d), %d)=%d\n", pthread_self(), a, *a, b , *a));
    return b;
}

static inline void cpu_relax()
{
    sched_yield();
}

#endif
