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

using namespace std;

atomic_flag v1_lock = ATOMIC_FLAG_INIT;
mutex v2_lock;

void thread_work_atomic(vector<int> *v) {
    for (int i = 0; i < 1000000; i++) {
        while (v1_lock.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        auto a = v->back();
        v->push_back(a + 1);
        v1_lock.clear(std::memory_order_release);
    }
}

void thread_work_mutex(vector<int> *v) {
    for (int i = 0; i < 1000000; i++) {
        v2_lock.lock();
        auto a = v->back();
        v->push_back(a + 1);
        v2_lock.unlock();
    }
}

int main() {
    int thread_num = 4;
    thread threads[thread_num];
    vector<int> vector1(1);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_num; i++) {
        threads[i] = std::thread(thread_work_atomic, &vector1);
    }
    for (int i = 0; i < thread_num; i++) {
        threads[i].join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << diff.count() << " s\n";
    std::cout << vector1.back() << endl;

    thread threads2[thread_num];
    vector<int> vector2(1);
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_num; i++) {
        threads2[i] = std::thread(thread_work_mutex, &vector2);
    }
    for (int i = 0; i < thread_num; i++) {
        threads2[i].join();
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << diff.count() << " s\n";
    std::cout << vector1.back() << endl;
}

