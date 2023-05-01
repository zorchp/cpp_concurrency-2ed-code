#include <__utility/integer_sequence.h>
#include <future>
#include <iostream>
#include <sys/wait.h>
using namespace std;


struct X {
    void foo(int, string const &s) { cout << s << endl; }
    string bar(string const &s) {
        cout << s << endl;
        return s;
    }
};

X x;
// 与 thread 传参方式类似
//  相当于调用 (&x)->foo(42,"hello");
auto f1 = async(&X::foo, &x, 42, "hello");
// tmp_x.bar("bye");
auto f2 = async(&X::bar, x, "bye");


struct Y {
    double operator()(double) { return 1.; }
};

Y y;
// 传入 函数对象, 实际上是一个临时对象(匿名变量)
auto f3 = async(Y(), 3.14);
// 传入具名对象(左值)
auto f4 = async(std::ref(y), 2.71828);
// baz 是函数
X baz(X &) {
    cout << "baz\n";
    return X();
}
// 需要包装成引用
auto f5 = async(baz, std::ref(x));

class move_only {
public:
    move_only() {}
    move_only(move_only &&) {}
    move_only(const move_only &) = delete;
    move_only &operator=(const move_only &) = delete;
    move_only &operator=(move_only &&rhs) { return *this; }
    void operator()() { cout << "callable\n"; }
};

// 调用临时对象, 其过程相当于std::move(move_only()), 与 f3 类似
auto f6 = async(move_only());

// 必须另外开启专属线程才能执行
auto f6_1 = async(launch::async, Y(), 12.3);

// 延后调用任务函数(deferred)
auto f7 = async(launch::deferred, baz, std::ref(x));

// 自行决定调用方式
auto f8 = async(launch::async | launch::deferred, baz, std::ref(x));
auto f9 = async(baz, std::ref(x));
/*
_LIBCPP_DECLARE_STRONG_ENUM(launch)
{
    async = 1,
    deferred = 2,
    any = async | deferred
};

any = 3
*/
int main(int argc, char *argv[]) {
    //
    f7.wait(); // 此时 f7 才开始执行
    return 0;
}
