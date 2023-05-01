#include <thread>
#include <vector>
#include <iostream>
#include <atomic>

// using namespace std;
// data 重定义了, 所以一定要用 std::命名空间
using namespace std::chrono_literals;

std::vector<int> data;
std::atomic<bool> data_ready(false);

void reader_thread() {
    while (!data_ready.load()) std::this_thread::sleep_for(1ms);
    std::cout << "ans=" << data[0] << '\n';
}

void write_thread() {
    data.emplace_back(42);
    data_ready = true;
}

int main(int argc, char *argv[]) {
    std::thread t(reader_thread);
    std::thread u(write_thread);
    t.join();
    u.join();
    return 0;
}
