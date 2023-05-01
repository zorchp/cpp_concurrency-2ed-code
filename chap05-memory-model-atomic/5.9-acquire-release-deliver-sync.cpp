#include <cassert>
#include <thread>
#include <atomic>

using namespace std;

namespace tt1 {
atomic_int Data[5];
atomic_bool sync1(false), sync2(false);

void t1() {
    Data[0].store(42, memory_order_relaxed);
    Data[1].store(32, memory_order_relaxed);
    Data[2].store(91, memory_order_relaxed);
    Data[3].store(2003, memory_order_relaxed);
    Data[4].store(-1, memory_order_relaxed);
    sync1.store(true, memory_order_release);
}

void t2() {
    while (!sync1.load(memory_order_acquire))
        ;
    sync2.store(true, memory_order_release);
}

void t3() {
    while (!sync2.load(memory_order_acquire))
        ;
    assert(Data[0].load(memory_order_relaxed) == 42);
}
} // namespace tt1


namespace tt2 {
atomic_int Data[5], sync(0); // 仅用单一变量作为信号

void t1() {
    Data[0].store(42, memory_order_relaxed);
    Data[1].store(32, memory_order_relaxed);
    Data[2].store(91, memory_order_relaxed);
    Data[3].store(2003, memory_order_relaxed);
    Data[4].store(-1, memory_order_relaxed);
    sync.store(1, memory_order_release);
}

void t2() {
    int expected = 1;
    while (!sync.compare_exchange_strong(expected, 2, memory_order_acq_rel))
        expected = 1;
}

void t3() {
    while (sync.load(memory_order_acquire) < 2)
        ;
    assert(Data[0].load(memory_order_relaxed) == 42);
}
} // namespace tt2

void test1() {
    // using namespace tt1;
    using namespace tt2;
    thread T1(t1), T2(t2), T3(t3);
    T1.join();
    T2.join();
    T3.join();
}

int main(int argc, char *argv[]) {
    test1();
    return 0;
}
