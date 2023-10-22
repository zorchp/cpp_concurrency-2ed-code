#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <format>
#include <condition_variable>

constexpr int BUF_SIZE = 10;
constexpr int MAX_NUM = 15; // number of goods

// 目前仅支持单一生产者和单一消费者
// 如果使用多个会导致这多个生产者(消费者)之间出现数据竞争, 导致线程空转
constexpr int PRODUCER_NUM = 1;
constexpr int CONSUMER_NUM = 1;

// thread sync tool
std::mutex mtx;
std::condition_variable cv_not_full; // buf 不满, 生产者等待此条件成立
std::condition_variable cv_not_empty; // buf 不空, 消费者等待此条件成立

// data buffer: use vector instead of queue, because queue can not specify size
std::vector<int> buf(BUF_SIZE);

// data ptr:
static size_t read_pos = 0;
static size_t write_pos = 0;
// 仅使用上面的两个指针就可以完成信号的通知机制, 但是不太好理解,
// 这里用 cnt 来判断比较清晰
[[maybe_unused]] static size_t cnt = 0;

void produce_one(int i) { // Product one , idx = i
    std::unique_lock lk(mtx);
    // while ((write_pos + 1) % BUF_SIZE == read_pos) {
    //     std::cout << "producer is waiting for an unfilled buf\n";
    //     cv_not_full.wait(lk);
    // }

    // equals to
    cv_not_full.wait(lk, [] {
        std::cout << "check buf is not full\n";
        // return (write_pos + 1) % BUF_SIZE != read_pos;
        return cnt != MAX_NUM;
    });

    buf[write_pos] = i;

    write_pos = (write_pos + 1) % BUF_SIZE;
    cv_not_empty.notify_one(); // 通知消费者, 不空, 可以开始消费
    ++cnt;
}

int consume_one() {
    std::unique_lock lk(mtx);
    // while (write_pos == read_pos) {
    //     std::cout << "consumer is waiting for items\n";
    //     cv_not_empty.wait(lk);
    // }

    cv_not_empty.wait(lk, [] {
        std::cout << "check buf is not empty\n";
        // return write_pos != read_pos;
        return cnt != 0;
    });
    auto data = buf[read_pos];
    read_pos = (read_pos + 1) % BUF_SIZE;
    cv_not_full.notify_one();
    --cnt;
    return data;
}

void producer() {
    for (int i{}; i < MAX_NUM; ++i) {
        std::cout << std::format("生产第 {} 个\n", i);
        produce_one(i);
    }
}

void consumer() {
    int i{};
    for (;;) {
        std::cout << std::format("消费第 {} 个\n", consume_one());
        if (++i == MAX_NUM) {
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::thread> vp, vc;
    for (int i{}; i < PRODUCER_NUM; ++i) {
        vp.emplace_back(producer);
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
