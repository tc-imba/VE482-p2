#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <memory>

using namespace std;

int main() {

    auto ptr = make_shared<int>(1);
    cout << ptr.use_count() << endl;
    auto ptr2 = shared_ptr<int>(ptr.get());
    cout << ptr2.use_count() << endl;

}