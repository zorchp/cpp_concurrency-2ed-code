#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>

using namespace std;

thread::id master_thread_id;

void do_work(unsigned id) {
    if (master_thread_id == this_thread::get_id())
        cout << "master\n";
    else {
        cout << "this_thread::get_id()=" << this_thread::get_id() << endl;
        cout << "id=" << id << endl;
        sleep(1);
    }
}

void t1() {
    cout << thread::hardware_concurrency() << endl; // 8
}


void t2() {


    cout << "master_thread_id=" << master_thread_id << endl;
    sleep(1);
    vector<thread> ts;
    for (unsigned i{}; i < 10; ++i) {
        ts.emplace_back(do_work, i);
        sleep(1);
    }

    for (auto &entry : ts) { entry.join(); }
}
// clang++ -lpthread 2.10-get-thread-id.cpp
// master_thread_id=0x0
// this_thread::get_id()=0x16f6df000
// id=0
// this_thread::get_id()=0x16f76b000
// id=1
// this_thread::get_id()=0x16f7f7000
// id=2
// this_thread::get_id()=0x16f883000
// id=3
// this_thread::get_id()=0x16f90f000
// id=4
// this_thread::get_id()=0x16f99b000
// id=5
// this_thread::get_id()=0x16fa27000
// id=6
// this_thread::get_id()=0x16fab3000
// id=7
// this_thread::get_id()=0x16fb3f000
// id=8
// this_thread::get_id()=0x16fbcb000
// id=9

int main(int argc, char const *argv[]) {
    // t1();
    t2();
    return 0;
}