#include <malloc/_malloc.h>
#include <memory>
#include <stack>
#include <future>
#include <list>
#include <iostream>
#include <thread>
#include <vector>
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


template <typename T>
struct sorter {
    struct chunk_to_sort {
        std::list<T> data;
        std::promise<std::list<T>> promise;
    };
    threadsafe_stack<chunk_to_sort> chunks;
    vector<std::thread> threads;
    unsigned const max_thread_count;
    std::atomic<bool> end_of_data;

    sorter()
        : max_thread_count(std::thread::hardware_concurrency() - 1),
          end_of_data(false) {}

    ~sorter() {
        end_of_data = true;
        for (unsigned i{}; i < threads.size(); ++i) threads[i].join();
    }

    void try_sort_trunk() {
        shared_ptr<chunk_to_sort> chunk = chunks.pop();
        if (chunk) sort_chunk(chunk);
    }

    list<T> do_sort(list<T>& chunk_data) {
        //
        if (chunk_data.empty()) return chunk_data;
        list<T> res;
        res.splice(res.begin(), chunk_data, chunk_data.begin());
        T const& partition_val = *res.begin();
        typename list<T>::iterator divide_point =
            std::partition(chunk_data.begin(), chunk_data.end(),
                           [&](T const& val) { return val < partition_val; });
        chunk_to_sort new_lower_chunk;
        new_lower_chunk.data.splice(new_lower_chunk.end(), chunk_data,
                                    chunk_data.begin(), divide_point);
        future<list<T>> new_lower = new_lower_chunk.promise.get_future();
        chunks.push(move(new_lower_chunk));
        if (threads.size() < max_thread_count)
            threads.push_back(threads(&sorter<T>::sort_thread, this));

        list<T> new_higher(do_sort(chunk_data));
        res.splice(res.end(), new_higher);
        while (new_lower.wait_for(chrono::seconds(0)) != future_status::ready)
            try_sort_trunk();
        res.splice(res.begin(), new_lower.get());
        return res;
    }

    void sort_chunk(shared_ptr<chunk_to_sort> const& chunk) {
        chunk->promise.set_value(do_sort(chunk->data));
    }

    void sort_thread() {
        while (!end_of_data) {
            try_sort_trunk();
            this_thread::yield();
        }
    }
};

template <typename T>
list<T> parallel_quick_sort(list<T> input) {
    if (input.empty()) return input;
    sorter<T> s;
    return s.do_sort(input);
}


int main(int argc, char* argv[]) {
    //
    return 0;
}
