#include <unistd.h>
#include <iostream>
#include <thread>
using namespace std;

// 通过在析构函数中添加join, 使得子线程不会停止执行
// 标准的RAII手法，在其析构函数中调用join()
class thread_guard {
    std::thread& t;

public:
    explicit thread_guard(std::thread& t_) : t(t_) {}
    ~thread_guard() {
        // 判断是否可汇合, 因为每一个线程只能调用`join()`一次
        if (t.joinable()) t.join();
    }
    /* 限令编译器不得自动生成相关代码。复制这类对象或向其赋值均有可能带来问题，
    因为所产生的新对象的生存期也许更长，甚至超过了与之关联的线程。*/
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};

void do_1(int& tmp, unsigned& tmp1) { cout << tmp << " " << tmp1 << endl; }

struct func {
    int& i;
    func(int& i_) : i(i_) {}
    void operator()() {
        for (unsigned j = 0; j < 1000000; ++j) {
            do_1(i, j);
            sleep(1);
        }
    }
};

void f() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func);
    thread_guard g(t);
    cout << "current thread" << endl;
}

int main(int argc, char const* argv[]) {
    f();
    return 0;
}