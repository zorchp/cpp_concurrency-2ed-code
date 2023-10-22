#include <bits/stdc++.h>
using namespace std;


void t1() {
    atomic_int ai{12};
    cout << boolalpha;
    cout << ai.is_lock_free() << endl;
    cout << ai.is_always_lock_free << endl;
    ++ai;
    // ai *= 12; // 未重载*和 /
    // ai /= 12;
    ai &= 12;
    cout << ai.load();
}

int main(int argc, char *argv[]) {
    t1();
    return 0;
}
