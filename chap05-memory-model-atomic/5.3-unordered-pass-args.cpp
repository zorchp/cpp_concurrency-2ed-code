#include <iostream>
void foo(int a, int b) { std::cout << a << ", " << b << '\n'; }

int get_num() {
    static int num = 0;
    return ++num;
}

int main(int argc, char *argv[]) {
    foo(get_num(), get_num()); // 两次调用没有明确的先后次序
    return 0;
}
