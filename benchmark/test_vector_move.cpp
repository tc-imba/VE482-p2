//
// Created by 38569 on 2017/10/31.
//

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

using namespace std;

int main() {
    vector<pair<int, int> > vector1(10000000);
    for (auto &item:vector1) {
        item.first = rand();
        item.second = rand();
    }
    auto c1 = clock();
    for (auto &item:vector1) {
        sin(log(item.first) + item.second);
        log(sin(item.second) + item.first);
    }
    cout << clock() - c1 << endl;
    vector<pair<int, int> > vector2;
    vector2.reserve(vector1.size());
    c1 = clock();
    for (auto &item:vector1) {
        sin(log(item.first) + item.second);
        log(sin(item.second) + item.first);
        vector2.push_back(move(item));
    }
    cout << clock() - c1 << endl;
    return 0;
}


