#include <iostream>
#include <thread>

using namespace std;

void f() { cout << "f()\n"; }
void g() { cout << "g()\n"; }

void test1() {
    thread t1(f);         // t1:f
    t1.join();
    thread t2 = move(t1); // t2:f
    t1 = thread(g);       // t1:g
    t1.join();
    thread t3;
    t3 = move(t2); // t1:g t2:∅ t3:f
    // 运行f的线程归属权转移到t1, 该线程最初由t1启动, 但是在转移时,
    // t1已经关联到g的线程, 因此terminate()会被调用, 终止程序.
    t1 = move(t3); // 终止整个程序
    // f()
    // g()
}

// f1() 用于生成线程实例
thread f1() { return thread(f); }

// g1() 用于生成线程实例
thread g1() {
    thread t([](int x) { cout << x << endl; }, 42);
    return t;
}

void f3(thread t) {}
void g3() {
    // 线程归属权可以转移到函数内部, 函数能够接收thread实例作为按右值传递的参数.
    f3(thread(f));
    thread t(f);
    f3(std::move(t));
}

int main(int argc, char const *argv[]) {
    test1();
    return 0;
}
