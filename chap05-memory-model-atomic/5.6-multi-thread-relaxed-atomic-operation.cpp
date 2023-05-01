#include <thread>
#include <atomic>
#include <iostream>

std::atomic<int> x(0), y(0), z(0);
std::atomic<bool> go(false);
unsigned const loop_cnt{10};

struct read_values {
    int x, y, z;
};


read_values v1[loop_cnt];
read_values v2[loop_cnt];
read_values v3[loop_cnt];
read_values v4[loop_cnt];
read_values v5[loop_cnt];

void inc(std::atomic<int>* var_to_inc, read_values* vals) {
    while (!go) std::this_thread::yield();

    for (unsigned i = 0; i < loop_cnt; ++i) {
        vals[i].x = x.load(std::memory_order_relaxed);
        vals[i].y = y.load(std::memory_order_relaxed);
        vals[i].z = z.load(std::memory_order_relaxed);
        var_to_inc->store(i + 1, std::memory_order_relaxed);
        std::this_thread::yield();
    }
}


void read_vals(read_values* vals) {
    //
    while (!go) std::this_thread::yield();

    for (unsigned i = 0; i < loop_cnt; ++i) {
        vals[i].x = x.load(std::memory_order_relaxed);
        vals[i].y = y.load(std::memory_order_relaxed);
        vals[i].z = z.load(std::memory_order_relaxed);
        std::this_thread::yield();
    }
}

void print(read_values* v) {
    for (unsigned i = 0; i < loop_cnt; ++i) {
        if (i) std::cout << ",";
        std::cout << "(" << v[i].x << "," << v[i].y << "," << v[i].z << ")";
    }
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    //
    std::thread t1(inc, &x, v1);
    std::thread t2(inc, &y, v2);
    std::thread t3(inc, &z, v3);
    std::thread t4(read_vals, v4);
    std::thread t5(read_vals, v5);
    go = true;
    t5.join();
    t4.join();
    t3.join();
    t2.join();
    t1.join();
    print(v1);
    print(v2);
    print(v3);
    print(v4);
    print(v5);
    return 0;
}
/* 前i行i元素依次递增(i=1,2,3)
   递增幅度并不一样
(0,1,1),(1,1,10),(2,1,10),(3,1,10),(4,6,10),(5,6,10),(6,6,10),(7,6,10),(8,6,10),(9,8,10)
(0,0,1),(4,1,10),(4,2,10),(4,3,10),(4,4,10),(4,5,10),(9,6,10),(9,7,10),(10,8,10),(10,9,10)
(0,0,0),(0,1,1),(1,1,2),(1,1,3),(1,1,4),(1,1,5),(1,1,6),(1,1,7),(1,1,8),(1,1,9)
(4,1,10),(4,1,10),(4,1,10),(9,6,10),(9,6,10),(9,6,10),(9,6,10),(10,8,10),(10,8,10),(10,8,10)
(9,6,10),(9,6,10),(9,6,10),(9,6,10),(9,6,10),(9,7,10),(9,8,10),(10,8,10),(10,8,10),(10,8,10)
*/
