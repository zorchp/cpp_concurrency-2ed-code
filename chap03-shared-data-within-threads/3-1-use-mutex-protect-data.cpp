#include <iostream>
#include <list>
#include <mutex>
#include <algorithm>
using namespace std;


std::list<int> some_list; // 1
std::mutex some_mutex;    // 2

ostream& operator<<(ostream& os, const list<int>& l) {
    for (auto i : l) os << i << " ";
    return os << endl;
}

void add_to_list(int new_value) {
    std::lock_guard<std::mutex> guard(some_mutex); // 3
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find) {
    std::lock_guard<std::mutex> guard(some_mutex); // 4
    return std::find(some_list.begin(), some_list.end(), value_to_find) !=
           some_list.end();
}
void t1() {
    cout << some_list;
    add_to_list(1);
    cout << some_list;
}

int main(int argc, char const* argv[]) {
    t1();
    return 0;
}
