#include <future>
#include <iostream>
using namespace std;

int find_ans() { return 42; }


int main(int argc, char *argv[]) {
    //
    future<int> ans = std::async(find_ans);
    cout << "ans is " << ans.get() << endl; // ans is 42
    return 0;
}
