#include <iostream>
#include <thread>
#include <chrono>

void foo() { std::this_thread::sleep_for(std::chrono::seconds(1)); }

void bar() { std::this_thread::sleep_for(std::chrono::seconds(1)); }

int main() {
    std::thread t1(foo);
    std::thread t2(bar);

    std::cout << "thread 1 id: " << t1.get_id() << '\n'
              << "thread 2 id: " << t2.get_id() << '\n';

    std::swap(t1, t2);

    std::cout << "after std::swap(t1, t2):" << '\n'
              << "thread 1 id: " << t1.get_id() << '\n'
              << "thread 2 id: " << t2.get_id() << '\n';

    t1.swap(t2);

    std::cout << "after t1.swap(t2):" << '\n'
              << "thread 1 id: " << t1.get_id() << '\n'
              << "thread 2 id: " << t2.get_id() << '\n';

    t1.join();
    t2.join();
}
/*
thread 1 id: 0x16d8a7000
thread 2 id: 0x16d933000
after std::swap(t1, t2):
thread 1 id: 0x16d933000
thread 2 id: 0x16d8a7000
after t1.swap(t2):
thread 1 id: 0x16d8a7000
thread 2 id: 0x16d933000
*/