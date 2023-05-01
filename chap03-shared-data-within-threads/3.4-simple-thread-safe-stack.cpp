#include <thread>
#include <memory>
#include <exception>
using namespace std;


struct empty_stack : exception {
    const char* ehat() const noexcept;
};

template <typename T>
class threadsafe_stack {

public:
    threadsafe_stack();
    threadsafe_stack(const threadsafe_stack&);

    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

    void push(T new_val);
    shared_ptr<T> pop(); // method3, 智能指针, 指向弹出元素
    void pop(T& val);    // method1, 传入引用, 但是会浪费资源
    bool empty() const;
};

int main(int argc, char const* argv[]) {
    ;
    return 0;
}
