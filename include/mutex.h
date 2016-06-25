#ifndef MUTEX_H
#define MUTEX_H

#ifdef __cplusplus 
extern "C" { 
#endif

#include <stdint.h>

typedef int32_t mutex_t;

#define MUTEX_INITIALIZER UNLOCK

int32_t mutex_init(mutex_t *m);
int32_t mutex_destroy(mutex_t *m);

int32_t mutex_lock(mutex_t *m);
int32_t mutex_unlock(mutex_t *m);
int32_t mutex_trylock(mutex_t *m);

#if 0
#ifdef _DEBUG

#include <stdio.h>
#include <pthread.h>

#define DEBUG(str) str

#else /* else _DEBUG */

#define DEBUG(str)

#endif /* end _DEBUG */
#endif

#ifdef __cplusplus 
} 
#endif

#endif /* end MUTEX_H */
