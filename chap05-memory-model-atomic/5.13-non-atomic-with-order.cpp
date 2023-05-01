#include <atomic>
#include <cassert>
#include <thread>
#include <vector>
using namespace std::chrono_literals;

bool x;
std::atomic_bool y;
std::atomic_int z;

void write_x_then_y() {
    x = true;
    std::atomic_thread_fence(std::memory_order_release); // 释放栅栏
    y.store(true, std::memory_order_relaxed);
}

// 配对的栅栏才能完成同步 操作

void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed))
        ;
    std::atomic_thread_fence(std::memory_order_acquire); // 获取栅栏
    if (x) ++z;
}

int main(int argc, char *argv[]) {
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x_then_y), b(read_y_then_x);
    a.join();
    b.join();
    assert(z.load() != 0);
    return 0;
}
