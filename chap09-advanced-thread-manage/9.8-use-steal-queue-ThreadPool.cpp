#include <mutex>
#include <thread>
#include <deque>
#include <iostream>
#include <queue>
#include <vector>
#include <future>
using namespace std::chrono_literals;

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

    void push(data_type data) {
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


class ThreadPool {
    typedef function_wapper task_type;
    std::atomic_bool done;
    threadsafe_queue<task_type> pool_work_queue;

    std::vector<std::unique_ptr<work_stealing_queue>> queues;
    std::vector<std::thread> threads; // 工作线程存储位置
    join_threads joiner;

    static thread_local work_stealing_queue *local_work_queue;
    static thread_local unsigned my_index;

    void worker_thread(unsigned my_index_) {
        my_index = my_index_;
        local_work_queue = queues[my_index].get();
        while (!done) {
            run_pending_task();
        }
    }
    bool pop_task_from_local_queue(task_type &task) {
        return local_work_queue && local_work_queue->try_pop(task);
    }
    bool pop_task_from_pool_queue(task_type &task) {
        return pool_work_queue.try_pop(task);
    }
    bool pop_task_from_other_thread_queue(task_type &task) {
        for (unsigned i{}; i < queues.size(); ++i) {
            unsigned const index = (my_index + 1 + i) % queues.size();
            if (queues[index]->try_steal(task)) return true;
        }
        return false;
    }

public:
    ThreadPool() : done(false), joiner(threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        try {
            for (unsigned i{}; i < thread_count; ++i)
                queues.emplace_back(std::unique_ptr<work_stealing_queue>(
                    new work_stealing_queue));
            for (unsigned i{}; i < thread_count; ++i)
                threads.emplace_back(
                    std::thread(&ThreadPool::worker_thread, this, i));
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
        std::packaged_task<result_type()> task(f);
        std::future<result_type> ans(task.get_future());

        if (local_work_queue)
            local_work_queue->push(std::move(task));
        else
            pool_work_queue.push(std::move(task));
        return ans;
    }

    void run_pending_task() {
        task_type task;
        if (pop_task_from_local_queue(task) || pop_task_from_pool_queue(task) ||
            pop_task_from_other_thread_queue(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
};
thread_local work_stealing_queue *ThreadPool::local_work_queue;
thread_local unsigned ThreadPool::my_index;

int main(int argc, char *argv[]) {
    ThreadPool tp;
    for (auto i = 1; i < 15; ++i)
        std::cout << tp.submit([i] {
                           std::this_thread::sleep_for(i * .01s);
                           std::cout << "t" << i << '\n';
                           return i;
                       }).get();
    return 0;
}
