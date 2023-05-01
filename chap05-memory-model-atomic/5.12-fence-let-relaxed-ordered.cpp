#include <atomic>
#include <cassert>
#include <thread>
#include <vector>
using namespace std::chrono_literals;

std::atomic_bool x, y;
std::atomic_int z;

void write_x_then_y1() {
    x.store(true, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_release); // 释放栅栏
    y.store(true, std::memory_order_relaxed);
}
// 这样不一定能保证同步
void write_x_then_y() {
    std::atomic_thread_fence(std::memory_order_release); // 释放栅栏
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_relaxed);
}
// 配对的栅栏才能完成同步 操作

void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed))
        ;
    std::atomic_thread_fence(std::memory_order_acquire); // 获取栅栏
    if (x.load(std::memory_order_relaxed)) ++z;
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
