#include <thread>
#include <mutex>
using namespace std;

class hierarchical_mutex {
    std::mutex internal_mutex;

    unsigned long const hierarchy_value;
    unsigned long previous_hierarchy_value;

    static thread_local unsigned long this_thread_hierarchy_value; // 1

    void check_for_hierarchy_violation() {
        if (this_thread_hierarchy_value <= hierarchy_value) // 2
        {
            throw std::logic_error("mutex hierarchy violated");
        }
    }

    void update_hierarchy_value() {
        previous_hierarchy_value = this_thread_hierarchy_value; // 3
        this_thread_hierarchy_value = hierarchy_value;
    }

public:
    explicit hierarchical_mutex(unsigned long value)
        : hierarchy_value(value), previous_hierarchy_value(0) {}

    void lock() {
        check_for_hierarchy_violation();
        internal_mutex.lock();    // 4
        update_hierarchy_value(); // 5
    }

    void unlock() {
        this_thread_hierarchy_value = previous_hierarchy_value; // 6
        internal_mutex.unlock();
    }

    bool try_lock() {
        check_for_hierarchy_violation();
        if (!internal_mutex.try_lock()) // 7
            return false;
        update_hierarchy_value();
        return true;
    }
};


thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(
    ULONG_MAX);                             // 7

hierarchical_mutex high_level_mutex(10000); // 1
hierarchical_mutex low_level_mutex(5000);   // 2

int do_low_level_stuff() { return 1; }

int low_level_func() {
    std::lock_guard<hierarchical_mutex> lk(low_level_mutex); // 3
    return do_low_level_stuff();
}

void high_level_stuff(int some_param) {}

void high_level_func() {
    std::lock_guard<hierarchical_mutex> lk(high_level_mutex); // 4
    high_level_stuff(low_level_func());                       // 5
}

void thread_a() // 6
{
    high_level_func();
}

void do_other_stuff() {}

void other_stuff() {
    high_level_func(); // 8
    do_other_stuff();
}

hierarchical_mutex other_mutex(6000);                    // 2

void thread_b()                                          // 9
{
    std::lock_guard<hierarchical_mutex> lk(other_mutex); // 10
    other_stuff();
}

int main(int argc, char const *argv[]) {
    //
    // thread_a(); // ok
    // thread_b(); // libc++abi: terminating with uncaught exception of type
    // std::logic_error: mutex hierarchy violated

    return 0;
}
