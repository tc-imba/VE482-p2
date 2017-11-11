//
// Created by 38569 on 2017/10/31.
//

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <chrono>

using namespace std;

int main() {
    size_t SIZE = 1000000;
    vector<pair<int, vector<int >>> v1(SIZE);
    vector<pair<int, vector<int >>> v2(SIZE);
    for (auto &item : v1) {
        item.first = rand();
        for (int i = 0; i < 100; i++) {
            item.second.emplace_back(1);
        }
    }
    for (auto &item : v2) {
        item.first = rand();
        for (int i = 0; i < 100; i++) {
            item.second.emplace_back(1);
        }
    }
    auto comp = [](const pair<int, vector<int >> &a, const pair<int, vector<int >> &b) {
        return a.first < b.first;
    };
    sort(v1.begin(), v1.end(), comp);
    sort(v2.begin(), v2.end(), comp);
    vector<pair<int, vector<int >>> v3(v1);
    vector<pair<int, vector<int >>> v4(v2);

    auto start = std::chrono::high_resolution_clock::now();

    auto size = v1.size();
    move(v2.begin(), v2.end(), back_inserter(v1));
    auto mid = v1.begin() + size;
    inplace_merge(v1.begin(), mid, v1.end(), comp);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << diff.count() << " s\n";

    start = std::chrono::high_resolution_clock::now();

    mid = v3.insert(v3.end(), v4.begin(), v4.end());
    inplace_merge(v3.begin(), mid, v3.end(), comp);

    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << diff.count() << " s\n";

    return 0;
}


