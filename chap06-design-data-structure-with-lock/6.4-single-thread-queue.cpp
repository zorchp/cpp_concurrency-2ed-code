#include <iostream>
#include <future>
#include <condition_variable>
#include <memory>

using namespace std;

namespace single_thread_queue {

template <typename T>
class queue {
private:
    struct node {
        T data;
        unique_ptr<node> next;
        node(T data_) : data(std::move(data_)) {}
    };
    unique_ptr<node> head;
    node* tail;

public:
    queue() : tail(nullptr) {}
    queue(const queue& rhs) = delete;
    queue& operator=(const queue& rhs) = delete;

    shared_ptr<T> try_pop() {
        if (!head) return shared_ptr<T>();
        shared_ptr<T> const ans(make_shared<T>(std::move(head->data)));
        unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);
        if (!head) tail = nullptr;
        return ans;
    }

    void push(T new_val) {
        unique_ptr<node> p(new node(std::move(new_val)));
        node* const new_tail = p.get();
        if (tail)
            tail->next = std::move(p);
        else
            head = std::move(p);
        tail = new_tail;
    }
};
} // namespace single_thread_queue
