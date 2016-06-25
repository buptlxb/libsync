#ifndef ATOMIC_H
#define ATOMIC_H

#define UNLOCK 0
#define LOCKED 1
#define LOCKED_WITH_WAITERS 2

#ifdef _DEBUG

#include <stdio.h>
#include <pthread.h>

#define DEBUG(str) str

#else /* else _DEBUG */

#define DEBUG(str)

#endif /* end _DEBUG */

#ifdef __cplusplus 
extern "C" { 
#endif

#include <sched.h>

static inline int32_t atomic_cmpxchg_32(int32_t *v, int32_t o, int32_t n)
{
    DEBUG(fprintf(stderr, "[%d] atomic_cmpxchg_32(0x%p(%d), %d, %d)\n", pthread_self(), v, *v, o, n));
    int32_t tmp = 0;
    __asm__ __volatile__("lock cmpxchg %[n], %[v]\n\t"
            :"=a"(tmp)
            :"a"(o), [n]"r"(n), [v]"m"(*v)
            :"cc", "memory"
            );
    DEBUG(fprintf(stderr, "[%d] atomic_cmpxchg_32(0x%p(%d), %d, %d)=%d\n", pthread_self(), v, *v, o, n, tmp));
    return tmp;
}

static inline int64_t atomic_cmpxchg_64(int64_t *v, int64_t o, int64_t n)
{
    DEBUG(fprintf(stderr, "[%d] atomic_cmpxchg_64(0x%p(%ld), %ld, %ld)\n", pthread_self(), v, *v, o, n));
    int64_t tmp = 0;
    __asm__ __volatile__("lock cmpxchg %[n], %[v]\n\t"
            :"=a"(tmp)
            :"a"(o), [n]"r"(n), [v]"m"(*v)
            :"cc", "memory"
            );
    DEBUG(fprintf(stderr, "[%d] atomic_cmpxchg_64(0x%p(%ld), %ld, %ld)=%ld\n", pthread_self(), v, *v, o, n, tmp));
    return tmp;
}


static inline int32_t xchg_32(int32_t *a, int32_t b)
{
    DEBUG(fprintf(stderr, "[%d] xchg_32(0x%p(%d), %d)\n", pthread_self(), a, *a, b));
    __asm__ __volatile__("xchg %[b], %[a]\n\t"
            :[b]"+r"(b):[a]"m"(*a)
            :"memory"
            );
    DEBUG(fprintf(stderr, "[%d] xchg(0x%p(%d), %d)=%d\n", pthread_self(), a, *a, b , b));
    return b;
}

static inline int32_t atomic_xchg_32(int32_t *a, int32_t b)
{
    DEBUG(fprintf(stderr, "[%d] xchg_32(0x%p(%d), %d)\n", pthread_self(), a, *a, b));
    __asm__ __volatile__("lock xchg %[b], %[a]\n\t"
            :[b]"+r"(b):[a]"m"(*a)
            :"memory"
            );
    DEBUG(fprintf(stderr, "[%d] xchg(0x%p(%d), %d)=%d\n", pthread_self(), a, *a, b , b));
    return b;
}

static inline void cpu_relax()
{
    sched_yield();
}

static inline int32_t atomic_add_32(int32_t *a, int32_t b)
{
    DEBUG(fprintf(stderr, "[%d] atomic_add_32(0x%p(%d), %d)\n", pthread_self(), a, *a, b));
    __asm__ __volatile__("lock xadd %[b], %[a]\n\t"
            :[b]"+r"(b):[a]"m"(*a)
            :"memory","cc"
            );
    DEBUG(fprintf(stderr, "[%d] atomic_add_32(0x%p(%d), %d)=%d\n", pthread_self(), a, *a, b , b));
    return b;
}

#ifdef __cplusplus 
} 
#endif

#endif
