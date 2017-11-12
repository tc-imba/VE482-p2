//
// Created by liu on 2017/10/27.
//

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>

using namespace std;

unordered_map<string, int> map1;
unordered_map<string, int> map2;
vector<string> vec;
mutex map1_lock;

atomic_flag v1_lock = ATOMIC_FLAG_INIT;
mutex v2_lock;


string construct_string(int i) {
    string str;
    for (int j = 0; j < 2; j++) {
        str += to_string(i);
    }
    return str;
}

void thread_work_lock(vector<string>::iterator begin, vector<string>::iterator end) {
    for (auto i = begin; i < end; ++i) {
        //map1_lock.lock();
        map1.erase(*i);
        //map1_lock.unlock();
    }
}

void thread_work_no_lock(vector<string>::iterator begin, vector<string>::iterator end, unordered_set<string> *set1) {
    for (auto i = begin; i < end; ++i) {
        set1->emplace(*i);
    }
}

int main() {
    int thread_num = 4;
    thread threads[thread_num];
    int begin = 1000000;
    int size = 1000000;
    for (int i = begin; i < begin + size; i++) {
        auto str = construct_string(i);
        vec.push_back(str);
        map1.emplace(str, i);
        map2.emplace(str, i);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_num; i++) {
        threads[i] = std::thread(thread_work_lock, vec.begin() + size / thread_num * i,
                                 vec.begin() + size / thread_num * (i + 1));
    }
    for (int i = 0; i < thread_num; i++) {
        threads[i].join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << diff.count() << " s\n";

    thread threads2[thread_num];
    unordered_set<string> sets[thread_num];

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_num; i++) {
        threads2[i] = std::thread(thread_work_no_lock, vec.begin() + size / thread_num * i,
                                  vec.begin() + size / thread_num * (i + 1), sets + i);
    }
    for (int i = 0; i < thread_num; i++) {
        threads2[i].join();
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << diff.count() << " s\n";

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_num; i++) {
        for_each(sets[i].begin(), sets[i].end(), [](const string &i) {
            map2.erase(i);
        });
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << diff.count() << " s\n";
}

