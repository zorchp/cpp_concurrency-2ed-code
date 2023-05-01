#include <thread>
#include <mutex>


// 这里的std::lock()需要包含<mutex>头文件
class some_big_object {};

void swap(some_big_object& lhs, some_big_object& rhs);

class X {
private:
    some_big_object some_detail;
    std::mutex m;

public:
    X(some_big_object const& sd) : some_detail(sd) {}

    // 执行内部数据互换(避免死锁)
    friend void swap(X& lhs, X& rhs) {
        if (&lhs == &rhs) return;
        std::lock(lhs.m, rhs.m); // 1
        // mutex 作为第一参数, adopt_lock 指明互斥已被锁住, 即互斥上有锁存在,
        // lock_guard实例应当据此接收锁的所有权, 不得在构造函数内试图另行加锁
        std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock); // 2
        std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock); // 3
        swap(lhs.some_detail, rhs.some_detail);
    }
    // c++17 scoped_lock version
    friend void swap_17(X& lhs, X& rhs) {
        if (&rhs == &lhs) return;
        // std::scoped_lock guard(lhs.m, rhs.m);
        std::scoped_lock<std::mutex, std::mutex> guard(lhs.m, rhs.m);
        swap(lhs.some_detail, rhs.some_detail);
    }
};

void t1() {}


int main(int argc, char const* argv[]) {
    t1();
    return 0;
}
