#include <bits/stdc++.h>
using namespace std;

// stack 接口

template <typename T, typename Container = std::deque<T>>
class Stack {
public:
    explicit Stack(const Container&);
    explicit Stack(Container&& = Container());

    template <class Alloc>
    explicit Stack(const Alloc&);

    template <class Alloc>
    Stack(const Container&, const Alloc&);

    template <class Alloc>
    Stack(Container&&, const Alloc&);

    template <class Alloc>
    Stack(Stack&&, const Alloc&);

    bool empty() const;
    size_t size() const;
    T& top();
    T const& top() const;
    void push(T const&);
    void push(T&&);
    void pop();
    void swap(Stack&&);
    template <class... Args>
    void emplace(Args&&... args);
};
