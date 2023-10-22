#include <iostream>
#include <future>
#include <condition_variable>
#include <memory>
#include <mutex>

using namespace std;


template <typename T>
class threadsafe_queue {
private:
    struct node {
        shared_ptr<T> data;
        unique_ptr<node> next;
    };
    std::mutex head_mutex;
    std::mutex tail_mutex;
    unique_ptr<node> head;
    node* tail;

    node* get_tail() {
        lock_guard<mutex> tail_lock(tail_mutex);
        return tail;
    }

    unique_ptr<node> pop_head() {
        lock_guard<mutex> head_lock(head_mutex);
        if (head.get() == get_tail())
            return nullptr;

        unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

public:
    threadsafe_queue() : head(new node), tail(head.get()) {
    }
    threadsafe_queue(const threadsafe_queue& rhs) = delete;
    threadsafe_queue& operator=(const threadsafe_queue& rhs) = delete;

    shared_ptr<T> try_pop() {
        unique_ptr<node> old_head = pop_head();
        return old_head ? old_head->data : shared_ptr<T>();
    }

    void push(T new_val) {
        shared_ptr<T> new_data(make_shared<T>(std::move(new_val)));
        unique_ptr<node> p(new node);
        node* const new_tail = p.get();

        lock_guard<mutex> tail_lock(tail_mutex);
        tail->data = new_data;
        tail->next = std::move(p);
        tail = new_tail;
    }
};
