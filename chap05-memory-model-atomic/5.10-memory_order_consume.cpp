#include <cassert>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
struct X {
    int i;
    std::string s;
};

std::atomic<X*> p;
std::atomic_int a;

void create_x() {
    X* x = new X;
    x->i = 42;
    x->s = "Hello";
    a.store(99, std::memory_order_relaxed); // 先载入整型
    p.store(x, std::memory_order_release);
}


void use_x() {
    X* x;
    while (!(x = p.load(std::memory_order_consume)))
        ;
    // std::this_thread::sleep_for(1ms);
    assert(x->i == 42);
    assert(x->s == "Hello");
    assert(a.load(std::memory_order_relaxed) == 99);
}


int main(int argc, char* argv[]) {
    std::thread t1(create_x);
    std::thread t2(use_x);
    t2.join();
    t1.join();
    return 0;
}
