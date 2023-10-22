#include <cassert>
#include <atomic>
#include <thread>
#include <iostream>

/*
memory_order_seq_cst :
编译器和 CPU 都必须严格遵循源码逻辑流程的先后顺序, 在相同的线程中, 以该操作为界,
其后方的任何操作不得重新编排到它前面, 而前方的任何操作不得重新编排到它后面,
其中'任何'是指带有任何内存标记的任何变量之上的任何操作
*/

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() { x.store(true, std::memory_order_seq_cst); }
void write_y() { y.store(true, std::memory_order_seq_cst); }

void read_x_then_y() {
    while (!x.load(std::memory_order_seq_cst))
        ;
    if (y.load(std::memory_order_seq_cst)) ++z;
}

void read_y_then_x() {
    while (!y.load(std::memory_order_seq_cst))
        ;
    if (x.load(std::memory_order_seq_cst)) ++z;
}

int main(int argc, char *argv[]) {
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);
    a.join();
    b.join();
    c.join();
    d.join();
    assert(z.load() != 0);
    std::cout << z.load();
    return 0;
}
