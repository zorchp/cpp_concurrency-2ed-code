#include <iostream>
#include <thread>
using namespace std;


int main() {
    cout << "null thread id: " << thread().get_id() << endl;
    cout << "null thread id: " << thread::id() << endl; // static func
    thread t1([] {
        cout << "Hello t1!\n";
        cout << "t1 thread id(use this_thread::get_id): "
             << this_thread::get_id() << endl;
    });
    cout << "main thread id: " << this_thread::get_id() << endl;
    cout << "t1 id(use t1.get_id): " << t1.get_id() << endl;
    t1.join();
    // null thread id: 0x0
    // null thread id: 0x0
    // main thread id: 0x1046d0580
    // t1 id(use t1.get_id): 0x16bc43000
    // Hello t1!
    // t1 thread id(use this_thread::get_id): 0x16bc43000
}