#include <mutex>
#include <iostream>
#include <thread>

using namespace std;

mutex mtx;
int x = 0;

void adder(int& p) {
    ++p;
}

void func() {
    scoped_lock<mutex> lk(mtx);
    for (int i{}; i < 100000; ++i)
        x = x + 1;
}

void t1() {
    thread t1(func);
    thread t2(func);
    t1.join();
    t2.join();
    cout << x;
}

int main(int argc, char* argv[]) {
    t1();
    return 0;
}
