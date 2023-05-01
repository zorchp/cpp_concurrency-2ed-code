#include <iostream>
#include <thread>
#include <string>
#include <cstring>

using namespace std;


void t1() {
    // 此处虽然传入了引用, 但是还是先将字符串字面量(const char*)传入,
    // 进入线程上下文之后才进行转换(为 string 类型)
    thread t([](int i, string const& s) { cout << i << s << endl; }, 10,
             "hello");
}

void oops() {
    //
    auto f = [](int i, string const& s) { cout << i << s << endl; };
    char buf[1024]; // 局部变量(自动变量 )
    snprintf(buf, 10, "%i", 100);
    // thread t(f, 3, buf); // buf 可能已销毁
    // 直接传入 buf 可能会出现安全性问题, 原因是参数传递本意是将 buf 隐式转换为
    // String, 再将其作为函数参数, 但转换不一定能及时开始(由于 thread
    // 的工作机制, 其构造函数需要原样复制所有传入的参数)
    thread t(f, 3, string(buf)); // 这样可以解决, 直接在传入之前进行构造
    t.detach();
}

// 传入一个非常量引用
class Widget {};
void oops_again() {
    auto f = [](int id, Widget& w) {};
    Widget w1;
    // 此时传入的 w1 是右值形式, move-only 型别, 因为非常量引用不能向其传递右值
    // thread t(f, 10, w1);
    thread t(f, 10, std::ref(w1));
    t.join();
}

class X {
public:
    void do_something() { cout << "do_something\n"; }
};
void t2() {
    X my_x;
    // 向某个类的成员函数设定为线程函数, 需要传入函数指针, 指向该成员函数
    thread t(&X::do_something, &my_x);
    // 若考虑到对象指针, 成员函数的第一个形参实际上是其第二个实参
    // 向线程函数 传入的第三个参数就是成员函数的第一个参数
}

void process(unique_ptr<X>){}
void t3(){
    unique_ptr<X> p(new X);
    p->do_something();
    thread t(process, std::move(p));// 通过 move 移交智能指针所指对象的控制权
}

int main(int argc, char const* argv[]) {
    // t1();
    // oops();
    // oops_again();
    // t2();
    t3();
    return 0;
}
