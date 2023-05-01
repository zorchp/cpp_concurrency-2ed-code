#include <iostream>
#include <thread>
using namespace std;

int i{};

void f() {
    int num = 10000;
    for (int n{}; n < num; ++n) i = i + 1;
}

int main(int argc, char *argv[]) {
    thread t1(f);
    thread t2(f);
    t1.join();
    t2.join();
    cout << i << endl; // 10719

    return 0;
}
