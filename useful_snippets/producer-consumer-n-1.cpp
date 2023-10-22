#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <format>
#include <sstream>
#include <condition_variable>

constexpr int BUF_SIZE = 3;
constexpr int MAX_NUM = 50; // number of goods

constexpr int PRODUCER_NUM = 5;
constexpr int CONSUMER_NUM = 1;

// thread sync tool
std::mutex mtx;
// mutex to protect cnt
std::mutex mtx_cnt;
std::condition_variable cv_not_full; // buf 不满, 生产者等待此条件成立
std::condition_variable cv_not_empty; // buf 不空, 消费者等待此条件成立

// data buffer: use vector instead of queue, because queue can not specify size
std::vector<int> buf(BUF_SIZE);

// data ptr:
static size_t read_pos = 0;
static size_t write_pos = 0;
[[maybe_unused]] static size_t cnt = 0;

static size_t producer_cnt = 0;

void produce_one(int i) { // Product one , idx = i
    std::unique_lock lk(mtx);

    // equals to
    cv_not_full.wait(lk, [] {
        std::cout << "check buf is not full\n";
        return cnt != MAX_NUM;
    });

    buf[write_pos] = i;

    write_pos = (write_pos + 1) % BUF_SIZE;
    cv_not_empty.notify_one(); // 通知消费者, 不空, 可以开始消费
    ++cnt;
}

int consume_one() {
    std::unique_lock lk(mtx);

    cv_not_empty.wait(lk, [] {
        std::cout << "check buf is not empty\n";
        return cnt != 0;
    });
    auto data = buf[read_pos];
    read_pos = (read_pos + 1) % BUF_SIZE;
    cv_not_full.notify_one();
    --cnt;
    return data;
}

void producer(int idx) {
    bool flg{};
    int i{};
    while (!flg) {
        std::lock_guard lk(mtx_cnt);
        if (producer_cnt < MAX_NUM) {
            ++producer_cnt;
            std::cout << std::format("#{} 生产第 {} 个\n", idx, i);
            produce_one(i);
            ++i;
        } else {
            flg = true;
        }
    }
}

void consumer() {
    for (int i{}; i < MAX_NUM; ++i) {
        std::cout << std::format("消费第 {} 个\n", consume_one());
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::thread> vp, vc;
    for (int i{}; i < PRODUCER_NUM; ++i) {
        vp.emplace_back(producer, i);
    }
    for (int i{}; i < CONSUMER_NUM; ++i) {
        vc.emplace_back(consumer);
    }
    for (auto &p : vp) {
        p.join();
    }
    for (auto &c : vc) {
        c.join();
    }
    return 0;
}
