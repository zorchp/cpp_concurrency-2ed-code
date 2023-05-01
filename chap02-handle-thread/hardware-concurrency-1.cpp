#include <iostream>
#include <thread>

int main(int argc, char const *argv[]) {
    // static method
    std::cout << std::thread::hardware_concurrency(); // 8
    return 0;
}