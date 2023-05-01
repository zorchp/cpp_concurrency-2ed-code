#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::literals;


void t1() {
    thread t([] {
        cout << "detached thread\n";
        this_thread::sleep_for(1s);
    });
    t.detach();
    // t.join();
    assert(!t.joinable());
    this_thread::sleep_for(1s);
    cout << "Main thread\n";
}


int main(int argc, char const* argv[]) {
    auto start = system_clock::now();
    t1();
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Time spent: "
         << double(duration.count()) * microseconds::period::num /
                microseconds::period::den
         << "s" << endl;
    // detached thread
    // Main thread
    // Time spent: 1.00508s
    return 0;
}
