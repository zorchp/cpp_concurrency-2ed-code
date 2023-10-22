#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
using namespace std::chrono_literals;

std::vector<int> queue_data;
std::atomic<int> count;

void populate_queue() {
    unsigned const number_of_items = 20;
    queue_data.clear();
    for (unsigned i{}; i < number_of_items; ++i) queue_data.push_back(i);
    count.store(number_of_items, std::memory_order_release);
}

void consume_queue_items() {
    bool flg{};
    while (!flg) {
        int item_index;
        if ((item_index = count.fetch_sub(1, std::memory_order_acquire)) <= 0) {
            // wait_for_more_items();
            std::this_thread::sleep_for(100ms);
            continue;
        }
        // process(queue_data[item_index - 1]);
        std::this_thread::sleep_for(100ms);
        flg = true;
    }
}

int main(int argc, char *argv[]) {
    std::thread a(populate_queue);
    std::thread b(consume_queue_items);
    std::thread c(consume_queue_items);
    a.join();
    b.join();
    c.join();

    return 0;
}
