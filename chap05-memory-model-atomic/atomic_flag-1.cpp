#include <atomic>
#include <iostream>
using namespace std;

void t1() {
    atomic_flag af = ATOMIC_FLAG_INIT; // false
    af.clear();
    cout << boolalpha << af.test_and_set() << endl;
    cout << boolalpha << af.test_and_set() << endl;
}

int main(int argc, char *argv[]) {
    //
    t1();
    return 0;
}
