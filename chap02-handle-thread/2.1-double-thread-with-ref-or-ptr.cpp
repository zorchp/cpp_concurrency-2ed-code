#include <unistd.h> //sleep

#include <iostream>
#include <thread>
using namespace std;

void do_1(int& tmp) { cout << tmp << endl; }

struct func {
    int& i;
    func(int& i_) : i(i_) {}
    void operator()() {
        for (unsigned j = 0; j < 1000000; ++j) {
            do_1(i); // 可能访问了悬空引用
            sleep(1);
        }
    }
};

void oops() {
    int locate = 10;
    func my_func(locate);
    thread my_th(my_func);
    // 不等待新线程结束,就销毁主线程,容易出问题,改成join()避免此问题
    // my_th.detach();
    my_th.join();
    // 但是使用join()过于简单粗暴, 直接完全等待会消耗好多系统资源.
}

int main(int argc, char const* argv[]) {
    oops();
    return 0;
}
