#include <stack>
#include <iostream>
#include <exception>
#include <mutex>

using namespace std;

struct empty_stack : exception {
    const char* what() const throw();
};


template <typename T>
class threadsafe_stack {
    stack<T> st;
    mutable mutex m;

public:
    threadsafe_stack() {}
    threadsafe_stack(const threadsafe_stack& rhs) {
        lock_guard<mutex> lock(rhs.m);
        st = rhs.st;
    }
    threadsafe_stack& operator=(const threadsafe_stack& rhs) = delete;

    void push(T new_val) {
        lock_guard<mutex> lock(m);
        st.push(std::move(new_val));
    }

    std::shared_ptr<T> pop() {
        lock_guard<mutex> lock(m);
        if (st.empty()) throw empty_stack();
        shared_ptr<T> const ans(make_shared<T>(std::move(st.top())));
        st.pop();
        return ans;
    }

    void pop(T& val) { // 传回要删除元素的引用
        lock_guard<mutex> lock(m);
        if (st.empty()) throw empty_stack();
        val = std::move(st.top());
        st.pop();
    }

    bool empty() const {
        lock_guard<mutex> lock(m);
        return st.empty();
    }
};
