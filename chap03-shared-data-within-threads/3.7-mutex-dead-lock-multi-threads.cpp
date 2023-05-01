#include <iostream>
#include <thread>
#include <mutex>

std::mutex mutex1, mutex2;

void ThreadA() {
    // 获取 mutex1 锁
    mutex1.lock();
    std::cout << "Thread A has mutex1" << std::endl;

    // 休眠 1s，模拟线程 A 执行其他操作
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 尝试获取 mutex2 锁
    std::cout << "Thread A is waiting for mutex2" << std::endl;
    mutex2.lock();
    std::cout << "Thread A has mutex2" << std::endl;

    // 释放 mutex2 锁
    mutex2.unlock();
    // 释放 mutex1 锁
    mutex1.unlock();
}

void ThreadB() {
    // 获取 mutex2 锁
    mutex2.lock();
    std::cout << "Thread B has mutex2" << std::endl;

    // 休眠 1s，模拟线程 B 执行其他操作
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 尝试获取 mutex1 锁
    std::cout << "Thread B is waiting for mutex1" << std::endl;
    mutex1.lock();
    std::cout << "Thread B has mutex1" << std::endl;

    // 释放 mutex1 锁
    mutex1.unlock();
    // 释放 mutex2 锁
    mutex2.unlock();
}

int main() {
    std::thread threadA(ThreadA);
    std::thread threadB(ThreadB);

    threadA.join();
    threadB.join();

    return 0;
}
