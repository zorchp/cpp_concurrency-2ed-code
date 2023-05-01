#include <mutex>
#include <iostream>
#include <thread>
using namespace std;
void t1();
void t2();
mutex m1, m2;

// 单线程死锁
void t1() {
    lock_guard<std::mutex> l1(m1);
    t2();
}
void t2() {
    lock_guard<std::mutex> l2(m2);
    t1();
}

int main(int argc, char *argv[]) {
    t1();
    t2();
    return 0;
}
