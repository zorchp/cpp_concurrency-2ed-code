#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;

class scoped_thread {
    thread t;

public:
    explicit scoped_thread(thread t_) : t(move(t_)) {
        if (!t.joinable()) throw logic_error("No thread");
    }
    ~scoped_thread() { t.join(); }
    scoped_thread(const scoped_thread&) = delete;
    scoped_thread& operator=(const scoped_thread&) = delete;
};

void do_1(int& tmp, int t1) { cout << tmp << " " << t1 << endl; }
struct func {
    int& i;
    func(int& i_) : i(i_) {}
    void operator()() {
        for (unsigned j = 0; j < 1000000; ++j) {
            do_1(i, j); // 可能访问了悬空引用
            sleep(1);
        }
    }
};

void f() {
    int local = 0;
    scoped_thread t{thread(func(local))};
    cout << "do_something_in_current_thread\n";
    // do_something_in_current_thread
    // 0 0
    // 0 1
    // 0 2
    // 0 3
}

int main(int argc, char const* argv[]) {
    f();
    return 0;
}