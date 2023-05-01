#include <iostream>
#include <list>
using namespace std;

template <class T>
ostream& operator<<(ostream& os, const list<T>& l) {
    for (auto& i : l) os << i << " ";
    return os << endl;
}
template <typename T>
list<T> sequential_quick_sort(list<T> input) {
    if (input.empty()) return input;
    list<T> ans{};
    ans.splice(ans.begin(), input, input.begin()); // 首元素放置给 ans
    T const& pivot = *ans.begin();

    // 分割点为插入 pivot 的位置
    auto divide_point = partition(input.begin(), input.end(),
                                  [&](T const& t) { return t < pivot; });

    list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

    auto new_lower(sequential_quick_sort(std::move(lower_part)));
    auto new_higher(sequential_quick_sort(std::move(input)));
    ans.splice(ans.end(), new_higher);
    ans.splice(ans.begin(), new_lower);
    return ans;
}


int main(int argc, char* argv[]) {
    list<int> l{3, 5, 1, 4, 3, 5, 5, 33, 5, 35, 656, 7, 68, 2, 23};
    cout << sequential_quick_sort(l);
    // 1 2 3 3 4 5 5 5 5 7 23 33 35 68 656
    return 0;
}
