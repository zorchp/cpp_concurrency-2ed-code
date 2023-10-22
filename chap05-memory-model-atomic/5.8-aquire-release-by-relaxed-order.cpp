#include <atomic>
#include <thread>
#include <cassert>
#include <iostream>

using namespace std;

atomic<bool> x, y;
atomic<int> z;

void write_x() { // 原子化存储(store)即为释放操作
    x.store(true, memory_order_release);
}
void write_y() { // 原子化存储(store)即为释放操作
    y.store(true, memory_order_release);
}

void read_x_then_y() { // 原子化载入(load)即为获取操作
    while (!x.load(memory_order_acquire))
        ;
    if (y.load(memory_order_acquire)) ++z;
}
void read_y_then_x() { // 原子化载入(load)即为获取操作
    while (!y.load(memory_order_acquire))
        ;
    if (x.load(memory_order_acquire)) ++z;
}

int main(int argc, char *argv[]) {
    x = false;
    y = false;
    z = 0;
    thread a(write_x);
    thread b(write_y);
    thread c(read_x_then_y);
    thread d(read_y_then_x);
    a.join(), b.join(), c.join(), d.join();
    assert(z.load() != 0);
    cout << z.load();
    return 0;
}
