
#include <iterator>
#include <mutex>
class Y {
private:
    int some_detail;
    mutable std::mutex m;
    int get_detail() const {
        // 加锁保护要读取的值
        // 仅使用一个锁
        std::lock_guard<std::mutex> lock_a(m);
        return some_detail;
    }

public:
    Y(int sd) : some_detail(sd) {}
    friend bool operator==(Y const &lhs, Y const &rhs) {
        if (&rhs == &lhs) return true;
        int const lhs_val = lhs.get_detail();
        int const rhs_val = rhs.get_detail();
        return lhs_val == rhs_val;
    }
};
