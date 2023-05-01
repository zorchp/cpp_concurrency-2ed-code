#include <iostream>
#include <future>
#include <condition_variable>
#include <memory>

using namespace std;

namespace dummy_queue {

template <typename T>
class queue {
private:
    struct node {
        shared_ptr<T> data;
        unique_ptr<node> next;
    };
    unique_ptr<node> head;
    node* tail;

public:
    queue() : head(new node), tail(head.get()) {}
    queue(const queue& rhs) = delete;
    queue& operator=(const queue& rhs) = delete;

    shared_ptr<T> try_pop() {
        if (head.get() == tail) return shared_ptr<T>();

        shared_ptr<T> const ans(head->data);
        unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);
        if (!head) tail = nullptr;
        return ans;
    }

    void push(T new_val) {
        shared_ptr<T> new_data(make_shared<T>(std::move(new_val)));
        unique_ptr<node> p(new node);
        tail->data = new_data;
        node* const new_tail = p.get();
        if (tail)
            tail->next = std::move(p);
        else
            head = std::move(p);
        tail = new_tail;
    }
};
} // namespace dummy_queue
