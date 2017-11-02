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

#include "../src/query_results.h"
#include "../src/query/query.h"

using namespace std;

void thread_task(vector<double>::iterator begin, vector<double>::iterator end) {
    for (; begin != end; ++begin) {
        *begin = log(sin(rand()));
    }
}

class Task {
    size_t offset, length;

public:
    typedef std::unique_ptr<Task> Ptr;
};

class Thread {
    std::thread _thread;
    Task::Ptr _task;

};

class ThreadManager {
    std::vector<Thread> threadPool;
    std::queue<Task::Ptr> tasks;
    std::queue<QueryResult> results;
public:
    explicit ThreadManager(const size_t &threadNum) {
        for (int i = 0; i < threadNum; i++) {
            threadPool.push_back(std::move(Thread()));
        }
    }

    void addTask(const Query &query) {
        tasks.push(make_unique<Task>());
    }
};

int main() {
    const int size = 10000000;
    const int thread_num = 4;
    vector<double> vector1(size);

    auto clock1 = clock();
    for (auto &item :vector1) {
        item = log(sin(rand()));
    }
    cout << "single thread: " << clock() - clock1 << endl;

    clock1 = clock();
    thread threads[thread_num];
    auto begin = vector1.begin();
    auto end = vector1.begin();
    advance(end, size / thread_num);
    for (int i = 0; i < thread_num; i++) {
        threads[i] = std::thread(thread_task, begin, end);
        begin = end;
        advance(end, size / thread_num);
    }
    for (auto &t: threads) {
        t.join();
    }
    cout << thread_num << " threads: " << clock() - clock1 << endl;

}
