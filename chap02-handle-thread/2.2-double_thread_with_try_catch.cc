#include <unistd.h>
#include <iostream>
#include <thread>
#include <cassert>

using namespace std;

void do_1(int& tmp, unsigned& tmp1) { cout << tmp << " " << tmp1 << endl; }

struct func {
    int& i;
    func(int& i_) : i(i_) {}
    void operator()() {
        for (unsigned j = 0; j < 10; ++j) {
            do_1(i, j); // 可能访问了悬空引用
            sleep(1);
        }
    }
};


/*为了防止因抛出异常而导致的应用程序终结，我们需要决定如何处理这种情况。
一般地，如果读者打算在没发生异常的情况下调用join()，那么就算出现了异常，
同样需要调用join()，以避免意外的生存期问题。*/
void f() {
    int error = 0;
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func); // 创建线程
    try {
        // assert(error == 10); // error
        throw 100; // 抛出异常
        cout << "current thread" << endl;
    } catch (int tt) {
        cout << "error message" << endl;
        t.join();
        cout << tt << endl;
        // cerr << e.what() << endl;
        throw 1;
    }
    t.join();
}
//出现异常仍然等待子线程结束, 而不是直接退出程序

int main(int argc, char const* argv[]) {
    f();
    return 0;
}