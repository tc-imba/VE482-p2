//
// Created by liu on 2017/10/27.
//

#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <thread>
#include <atomic>
#include <mutex>

//#include "../src/query_results.h"
//#include "../src/query/query.h"

using namespace std;
const size_t size = 10000000;
const size_t thread_num = 8;
const size_t size2 = size / thread_num;
atomic_int counter(0);
mutex counter_mutex;
int counter2 = 0;

void thread_task() {
    //int counter = 0;
    for (int i = 0; i < size2; i++) {
        ++counter;
    }
}

void thread_task2() {
    //int counter = 0;
    for (int i = 0; i < size2; i++) {
        counter_mutex.lock();
        ++counter2;
        counter_mutex.unlock();
    }
}

int main() {
    auto clock1 = clock();
    int counter = 0;
    for (int i = 0; i < size; i++) {
        ++counter;
    }
    cout << "single thread: " << clock() - clock1 << endl;
    cout << counter << endl;

    clock1 = clock();
    thread threads[thread_num];
    for (int i = 0; i < thread_num; i++) {
        threads[i] = std::thread(thread_task);
    }
    for (int i = 0; i < thread_num; i++) {
        threads[i].join();
    }
    cout << thread_num << " threads: " << (clock() - clock1) / thread_num << endl;
    cout << counter << endl;

    clock1 = clock();
    thread threads2[thread_num];
    for (int i = 0; i < thread_num; i++) {
        threads2[i] = std::thread(thread_task2);
    }
    for (int i = 0; i < thread_num; i++) {
        threads2[i].join();
    }
    cout << thread_num << " threads: " << (clock() - clock1) / thread_num << endl;
    cout << counter << endl;
}

/*
#include <pthread.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <sys/time.h>
#include <zconf.h>

struct thread_info
{
    pthread_t tid;
    int id;
    unsigned seed;
    unsigned* hash;
    int64_t count;
};

const int HASH_SIZE = 5 * 1024 * 1024;
const int64_t TOTAL_OPS = 1000 * 1000 * 1000;

double now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void* thread_func(void* arg)
{
    struct thread_info ti = *(struct thread_info*)arg;
    printf("%f thread: %d started\n", now(), ti.id);

    for (int64_t i = 0; i < ti.count; ++i)
    {
        unsigned int obj = rand_r(&ti.seed);
        ti.hash[obj % HASH_SIZE] = obj; // rand_r(&ti.seed);
    }

    printf("%f thread: %d finished\n", now(), ti.id);
    return NULL;
}

int main(int argc, char* argv[])
{
    struct thread_info thread_infos[32];
    bzero(thread_infos, sizeof thread_infos);

    int num = sysconf(_SC_NPROCESSORS_CONF);
    printf("system has %d processor(s)\n", num);

    srand(time(NULL));

    int nThreads = argc > 1 ? atoi(argv[1]) : 1;
    double start = now();
    printf("%f creating %d threads\n", start, nThreads);
    for (int i = 0; i < nThreads; i++)
    {
        thread_infos[i].id = i;
        thread_infos[i].seed = rand();
        thread_infos[i].hash = (unsigned int *) calloc(HASH_SIZE, sizeof(unsigned int));
        thread_infos[i].count = TOTAL_OPS;
        pthread_create(&thread_infos[i].tid, NULL, thread_func, &thread_infos[i]);
    }

    for (int i = 0; i < nThreads; i++)
    {
        pthread_join(thread_infos[i].tid, NULL);
    }
    double end = now();
    double seconds = end - start;
    printf("%f all threads stopped, elapsed %f seconds\n", end, seconds);
    printf("total_ops/s = %.3f M\nops/s for each thread = %.3f M\n",
           TOTAL_OPS / seconds / 1000000 * nThreads,
           TOTAL_OPS / seconds / 1000000);
    return 0;
}
*/
