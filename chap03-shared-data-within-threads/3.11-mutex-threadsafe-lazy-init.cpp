#include <thread>
#include <memory>
#include <mutex>
using namespace std;
class P {
public:
    void do_something() {}
};

std::shared_ptr<P> res_ptr;
std::mutex res_mutex;

void foo() {
    // 让每一次对共享数据的访问都循序进行
    std::unique_lock<std::mutex> lk(res_mutex);
    // 尽管只有这里需要保护(初始化数据)
    if (!res_ptr) res_ptr.reset(new P);

    lk.unlock();
    res_ptr->do_something();
}

void undefined_behavior_with_double_checked_locking() {
    // 双重检验锁定模式
    // 无锁条件下读取指针,
    if (!res_ptr) {
        // 读取到空指针才继续获取锁
        std::lock_guard<std::mutex> lk(res_mutex);
        // 当前线程先判别空指针, 随即加锁
        // 但是这两步骤之间存在空隙, 所以其他线程或许正好借机完成初始化
        // 再次检验空指针(双重检验) 以防止此情况发生
        if (!res_ptr) {
            res_ptr.reset(new P);
        }
    }
    res_ptr->do_something();
}

void t1() {
    std::thread th1(foo);
    std::thread th2(foo);
    th1.join();
    th2.join();
}

void t2() {
    std::thread th1(undefined_behavior_with_double_checked_locking);
    std::thread th2(undefined_behavior_with_double_checked_locking);
    th1.join();
    th2.join();
}

int main(int argc, char *argv[]) {
    // t1();
    t2();
    return 0;
}
