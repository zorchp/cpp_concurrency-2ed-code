#include <mutex>
#include <thread>
#include <deque>
#include <future>

class function_wapper {
    struct impl_base {
        virtual void call() = 0;
        virtual ~impl_base() {}
    };
    std::unique_ptr<impl_base> impl;

    template <typename F>
    struct impl_type : impl_base {
        F f;
        impl_type(F &&f_) : f(std::move(f_)) {}
        void call() { f(); }
    };

public:
    template <typename F>
    function_wapper(F &&f) : impl(new impl_type<F>(std::move(f))) {}
    void operator()() { impl->call(); }
    function_wapper() = default;
    function_wapper(function_wapper &&rhs) : impl(std::move(rhs.impl)) {}
    function_wapper &operator=(function_wapper &&rhs) {
        impl = std::move(rhs.impl);
        return *this;
    }
    function_wapper(function_wapper &) = delete;
    function_wapper(const function_wapper &) = delete;
    function_wapper &operator=(const function_wapper &) = delete;
};

class work_stealing_queue {
private:
    typedef function_wapper data_type;
    std::deque<data_type> the_queue;
    mutable std::mutex the_mutex;

public:
    work_stealing_queue() {}
    work_stealing_queue(const work_stealing_queue &) = delete;
    work_stealing_queue &operator=(const work_stealing_queue &) = delete;

    void push_data(data_type data) {
        std::lock_guard<std::mutex> lock(the_mutex);
        the_queue.push_front(std::move(data));
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_queue.empty();
    }

    bool try_pop(data_type &res) {
        std::lock_guard<std::mutex> lock(the_mutex);
        if (the_queue.empty()) return false;
        res = std::move(the_queue.front());
        the_queue.pop_front();
        return true;
    }

    bool try_steal(data_type &res) {
        std::lock_guard<std::mutex> lock(the_mutex);
        if (the_queue.empty()) return false;
        res = std::move(the_queue.back());
        the_queue.pop_back();
        return true;
    }
};
