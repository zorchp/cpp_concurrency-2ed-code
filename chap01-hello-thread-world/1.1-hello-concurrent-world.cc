#include <iostream>
#include <thread>
using namespace std;

void fun() { cout << "Hello t1!\n"; }

void t1() {
    thread t1(&fun); // 传入函数指针
    cout << "main thread id: " << this_thread::get_id() << endl;
    cout << "t1 id: " << t1.get_id() << endl;
    if (t1.joinable()) cout << "t1 is joinable\n", t1.join();
    // 令主线程等待子线程
    // main thread id: 0x105340580
    // t1 id: 0x16af0b000
    // t1 is joinable
    // Hello t1!
}

void t2() {
    thread t2([] { cout << "Hello t2!\n"; }); // lambda 表达式
    t2.join();                                // Hello t2!
}

struct Foo {
    void operator()() const { cout << "Hello t3!\n"; }
};

void t3() { // 传入临时函数对象
    // 二义性(烦人的分析机制), 参见Effective STL,
    // 即`只要C++语句有可能被解释成函数声明, 编译器就肯定将其解释为函数声明`
    // thread t3((foo())); // 由于存在函数指针二义性, 这里必须用圆括号包裹
    thread t3{Foo()}; // 同理, 这里用一致性初始化{}, 推荐这种方法
    t3.join();        // Hello t3!
}

struct Foo1 {
    void operator()() const { cout << "Hello t4!\n"; }
};

void t4() {
    Foo1 f;
    thread t4(f);
    t4.join(); // Hello t4!
}

void t5() {
    auto t5 = thread([] { cout << "Hello t5!\n"; });
    t5.join(); // Hello t5!
}

int main(int argc, char const* argv[]) {
    // t1();
    // t2();
    // t3();
    // t4();
    t5();

    return 0;
}
