#include <cassert>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

condition_variable cv;
bool done{};
mutex m;

bool wait_loop() {
    auto const timeout = chrono::steady_clock::now() + 500ms;
    unique_lock<mutex> lk(m);
    while (!done) {
        if (cv.wait_until(lk, timeout) == cv_status::timeout) break;
    }
    return done;
}

int main(int argc, char* argv[]) {
    assert(wait_loop());
    return 0;
}
