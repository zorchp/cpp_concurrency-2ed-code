#include <thread>
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore(int _n) : n(_n) {}
    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        if (!n) //
            cv.wait(lock, [this] { return n; });
        --n;
    }
    void post() {
        std::lock_guard lock(mtx); // C++17
        ++n;
        cv.notify_one();
    }

private:
    int n;
    std::mutex mtx;
    std::condition_variable cv;
};
