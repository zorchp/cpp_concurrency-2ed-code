
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
    // c++17 scoped_lock version
    friend void swap_17(X& lhs, X& rhs) {
        if (&rhs == &lhs) return;
        std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
        std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
        std::lock(lock_a, lock_b);
        swap(lhs.some_detail, rhs.some_detail);
    }
};
