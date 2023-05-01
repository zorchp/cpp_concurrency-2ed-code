#include <iostream>
#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
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

class ThreadPool {
    std::atomic_bool done;
    // 任务实例: void(void) 类型
    threadsafe_queue<std::function<void()>> work_queue;
    std::vector<std::thread> threads; // 工作线程存储位置
    join_threads joiner;

    void worker_thread() {
        while (!done) {
            std::function<void()> task;
            if (work_queue.try_pop(task))
                task();
            else
                std::this_thread::yield();
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
    void submit(FunctionType f) {
        work_queue.push(std::function<void()>(f));
    }
};
/*
   存在的几个问题:
   1. 不支持阻塞操作
   2. 可能出现死锁
   3. 乱序执行
   4. 同步执行
*/

int main(int argc, char *argv[]) {
    ThreadPool tp;
    for (auto i = 1; i < 10; ++i)
        tp.submit([i] {
            std::this_thread::sleep_for(i * .01s);
            std::cout << "t" << i << '\n';
        });
    return 0;
}
