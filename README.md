libsync
=======

## Introduction
**libsync** is a simple thread synchronization library for X86/X64 implemented in pure *C*.
**libsync** has the pthread-like API and supports mutex, conditional variable and barrier currently.


## Motivation
*futex* is a complicated system call. Implementing my own synchronization library may help to understand it.

## Example

    :::C
    // static initialization
    mutex_t mutex = MUTEX_INITIALIZER;
    // dynamic initialization
    mutex_init(&mutex);
    // lock
    mutex_lock(&mutex);
    // unlock
    mutex_unlock(&mutex);
    // try lock
    mutex_trylock(&mutex);
    // destruct mutex
    mutex_destroy(&mutex);
    :::

## Build

**libsync** build successfully on my platform (gcc version 4.7.1 Linux Kernel 3.6.8 X86_64)

    make clean && make

## Performance

![perf.png](http://www.gotit.sinaapp.com/assets/images/perf.png)

## Others

libsync provide two MACROS to tune the performance.

* **SPIN** is defined to make mutex_lock spin some times to acquire the lock instead of invoking futex_wait to sleep immediately.
* **SPIN_TIMES** is defined to make the user specify the spin times when SPIN is enabled;
