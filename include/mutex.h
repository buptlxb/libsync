#ifndef MUTEX_H
#define MUTEX_H

typedef int mutex;

#define MUTEX_INITIALIZER {0}

int mutex_init(mutex *m);
int mutex_destroy(mutex *m);

int mutex_lock(mutex *m);
int mutex_unlock(mutex *m);
int mutex_trylock(mutex *m);

#ifdef _DEBUG

#include <stdio.h>
#include <pthread.h>

#define DEBUG(str) str

#else /* else _DEBUG */

#define DEBUG(str)

#endif /* end _DEBUG */

#endif /* end MUTEX_H */
