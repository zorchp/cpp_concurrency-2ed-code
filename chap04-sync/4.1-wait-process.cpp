#include <bits/stdc++.h>
using namespace std;
mutex mut;
class Data_chunk {};
queue<Data_chunk> data_queue;
condition_variable data_cond;
bool isprepare{};

void data_preparation_thread() {
    while (isprepare) {
        Data_chunk const data = Data_chunk();
        {
            // RAII
            lock_guard<mutex> lk(mut);
            data_queue.emplace(data);
        }
        data_cond.notify_one();
    }
}

void process(Data_chunk d) {
    cout << "processing...\n"; //
}

void data_process_thread() {
    while (1) {
        //
        unique_lock<mutex> lk(mut);
        data_cond.wait(lk, [] { return !data_queue.empty(); });
        auto data = data_queue.front();
        data_queue.pop();
        lk.unlock();
        process(data);
        if (data_queue.empty()) break;
    }
}


int main(int argc, char* argv[]) {
    thread t1(data_process_thread);
    thread t2(data_preparation_thread);
    t1.join();
    t2.join();
    return 0;
}
