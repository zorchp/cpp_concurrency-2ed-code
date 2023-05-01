#include <atomic>
#include <mutex>

using namespace std;


class spinlock_mutex {
    atomic_flag flag;

public:
    spinlock_mutex() : flag ATOMIC_FLAG_INIT {}

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire))
            ;
    }

    void unlock() { flag.clear(std::memory_order_release); }
};

int main(int argc, char *argv[]) {
    spinlock_mutex a;
    return 0;
}
