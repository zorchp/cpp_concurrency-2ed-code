#include <iostream>
#include <thread>

using namespace std;

void foo();
void bar();

void foo() {
    thread t1(bar);
    t1.join();
    cout << "foo done" << endl;
}
void bar() {
    thread t2(foo);
    t2.join();
    cout << "bar done" << endl;
}

int main() {
    foo();
    bar();
    return 0;
}
