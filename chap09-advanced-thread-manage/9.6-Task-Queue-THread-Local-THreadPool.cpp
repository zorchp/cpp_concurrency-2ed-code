#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <future>
#include <condition_variable>
#include <type_traits>
#include <vector>
using namespace std::chrono_literals;

// from 6.2
template <typename T>
class threadsafe_queue {
private:
    mutable std::mutex mut; // 针对诸如empty()的const成员,
                            // 需要修改互斥以达到条件
    std::queue<T> data_queue;
    std::condition_variable data_cond;

public:
    threadsafe_queue() {}
    threadsafe_queue(const threadsafe_queue &other) {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue = other.data_queue;
    }
    threadsafe_queue &operator=(const threadsafe_queue &) = delete;
    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(std::move(new_value));
        data_cond.notify_one();
    }

    void wait_and_pop(T &value) { // 作为参数, 不返回, 而是修改参数作为返回值
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = std::move(data_queue.front());
        data_queue.pop();
    }
    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        std::shared_ptr<T> res(
            std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }
    bool try_pop(T &value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return false;
        value = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return std::shared_ptr<T>();
        std::shared_ptr<T> res(
            std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }
    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

class join_threads { // join thread by RAII
    std::vector<std::thread> &threads;

public:
    explicit join_threads(std::vector<std::thread> &threads_)
        : threads(threads_) {}

    ~join_threads() {
        for (unsigned long i{}; i < threads.size(); ++i)
            if (threads[i].joinable()) threads[i].join();
    }
};

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

typedef std::queue<function_wapper> local_queue_type;
class ThreadPool {
    std::atomic_bool done;
    // 任务实例: void(void) 类型
    threadsafe_queue<function_wapper> pool_work_queue;

    static thread_local std::unique_ptr<local_queue_type> local_work_queue;

    std::vector<std::thread> threads; // 工作线程存储位置
    join_threads joiner;

    void worker_thread() {
        local_work_queue.reset(new local_queue_type);
        while (!done) {
            run_pending_task();
        }
    }

public:
    ThreadPool() : done(false), joiner(threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        try {
            for (unsigned i{}; i < thread_count; ++i)
                threads.emplace_back(&ThreadPool::worker_thread, this);
        } catch (...) {
            done = true;
            throw;
        }
    }
    ~ThreadPool() { done = true; }

    template <typename FunctionType>
    std::future<typename std::result_of_t<FunctionType()>> submit(
        FunctionType f) {
        typedef typename std::result_of_t<FunctionType()> result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> ans(task.get_future());

        if (local_work_queue)
            local_work_queue->push(std::move(task));
        else
            pool_work_queue.push(std::move(task));
        return ans;
    }

    void run_pending_task() {
        function_wapper task;
        if (local_work_queue && !local_work_queue->empty()) {
            task = std::move(local_work_queue->front());
            local_work_queue->pop();
            task();
        } else if (pool_work_queue.try_pop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
};
thread_local std::unique_ptr<local_queue_type> ThreadPool::local_work_queue;

// 改进版: 使用者可以等待池内线程完成任务

int main(int argc, char *argv[]) {
    ThreadPool tp;
    for (int i{}; i < 15; ++i)
        std::cout << tp.submit([i] {
                           std::this_thread::sleep_for(i * .01s);
                           std::cout << "t" << i << '\n';
                           return i;
                       }).get();
    return 0;
}
