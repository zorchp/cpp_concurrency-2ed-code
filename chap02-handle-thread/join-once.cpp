#include <bits/stdc++.h>
using namespace std;


int main() {
    //
    thread t1([] { cout << "AA\n"; });
    t1.join();                     //"AA"
    cout << t1.joinable() << endl; // 0
    t1.join(); // libc++abi: terminating with uncaught exception of type
               // std::__1::system_error: thread::join failed: Invalid argument
}
