//
// Created by 38569 on 2017/11/1.
//

#include <queue>
#include <list>
#include <iostream>

using namespace std;

int main() {
    queue<pair<int, int> > q;
    list<pair<int, int> > l;
    for (int i = 0; i < 10000000; i++) {
        q.emplace(rand(), rand());
    }
    auto c = clock();
    while (!q.empty()) {
        l.push_back(move(q.front()));
        q.pop();
    }
    cout << clock() - c << endl;
    /*for (auto &i : l) {
        cout << i << endl;
    }*/
}