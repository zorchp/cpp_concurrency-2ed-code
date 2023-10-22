#include <cassert>
#include <iostream>
#include <atomic>

using namespace std;

void t1() {
    atomic<int *> aip;
    cout << boolalpha;
    cout << aip.is_lock_free() << endl; // true
    cout << aip.load();                 // 0x0
}

class Foo {};

void t2() {
    Foo arr[5];
    atomic<Foo *> p(arr);
    Foo *x = p.fetch_add(2);
    --p;
    ++p; // 重载的运算符
    assert(x == arr);
    assert(p.load() == &arr[2]);
    x = (p -= 1);
    assert(x == &arr[1]);
    assert(p.load() == &arr[1]);
}

int main(int argc, char *argv[]) {
    // t1();
    t2();
    return 0;
}
