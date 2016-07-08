#include <iostream>
#include <vector>
#include <pthread.h>
#include "mutex.h"
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

#define OPERATIONS 250000
#define REPEAT 5

int counter = 0;

void *do_work(void *arg) {
    mutex_t *mutex = (mutex_t *)arg;
    for(int i = 0; i < OPERATIONS; ++i){
        mutex_lock(mutex);
        ++counter;
        mutex_unlock(mutex);
    }
}

template<int Threads>
void bench_lock(){
    mutex_t mutex;
    mutex_init(&mutex);
    unsigned long throughput = 0;

    for(int i = 0; i < REPEAT; ++i){
        std::vector<pthread_t> threads;

        Clock::time_point t0 = Clock::now();

        for(int i = 0; i < Threads; ++i){
            pthread_t tid;
            pthread_create(&tid, 0, do_work, &mutex);
            threads.push_back(tid);
        }

        for(auto thread : threads){
            pthread_join(thread, 0);
        }

        Clock::time_point t1 = Clock::now();

        milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        throughput += (Threads * OPERATIONS) / ms.count();
    }
    mutex_destroy(&mutex);

    std::cout << "counter = " << counter << std::endl;
    std::cout << "lock with " << Threads << " threads throughput = " << (throughput / REPEAT) << std::endl;
}

#define bench(name)\
    name<1>();\
    name<2>();\
    name<4>();\
    name<8>();\
    name<16>();\
    name<32>();\
    name<64>();\
    name<128>();

int main(){
    bench(bench_lock);

    return 0;
}
