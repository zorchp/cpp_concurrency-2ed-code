#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
using namespace std;
// using namespace std::chrono_literals;


class Semaphore {
public:
    Semaphore(int count = 0) : count_(count) {}

    operator int() { return count_; }
    void Signal() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return count_ > 0; });
        --count_;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};


const int N = 100;
Semaphore mtx = 1;
Semaphore empty_buf = N;
Semaphore full_buf = 0;


void producer() {
    while (true) {
        empty_buf.Wait();
        cout << "producing...\n";
        this_thread::sleep_for(100ms);
        mtx.Wait();
        cout << mtx << endl;
        mtx.Signal();
        full_buf.Signal();
        //
    }
}
void consumer() {
    while (true) {
        full_buf.Wait();
        cout << "consuming...\n";
        this_thread::sleep_for(100ms);
        mtx.Wait();
        cout << mtx << endl;
        mtx.Signal();
        empty_buf.Signal();
        //
    }
}

int main(int argc, char *argv[]) {
    cout << mtx;
    thread tp(producer), tc(consumer);
    tc.join();
    tp.join();
    return 0;
}
