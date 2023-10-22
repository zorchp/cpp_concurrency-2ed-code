#include <iostream>
#include <atomic>
using namespace std;

void t1() {
    atomic_bool ab{};
    ab = true;
    ab.load();
    ab.store(false);
    cout << boolalpha << ab.load() << endl; // false
    cout << ab.is_lock_free() << endl;      // true
    bool x = ab.exchange(true, memory_order_relaxed);
    cout << x << endl; // false
    cout << ab.load() << endl;
}

void t2() {
    // compare and exchange:
    bool expected = false;
    atomic_bool ab{};
    // 当线程数量多于处理器数量, 这种情况下线程可能执行到中途因系统调度而切出,
    // 导致操作失败  需要使用 while 循环或者 strong 函数
    // strong 内部也是通过循环实现的, 如果在 strong 外面嵌套循环会导致二重循环,
    // 不利于性能提升
    // ab.compare_exchange_weak(expected, true);
    while (!ab.compare_exchange_weak(expected, true) and !expected)
        ;
    cout << boolalpha;
    cout << ab;
}

void t3() {
    atomic_bool ab;
    bool expected{};
    ab.compare_exchange_weak(expected, true, memory_order_acq_rel,
                             memory_order_acquire);
    // equal to
    // ab.compare_exchange_weak(expected, true, memory_order_acq_rel);
    cout << boolalpha;
    cout << ab;
}

int main() {
    // t1();
    // t2();
    t3();
}
