#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>

using namespace std;

void do_work(unsigned id) {
    cout << id << endl;
    sleep(1);
}


void t1() {
    vector<thread> ts;
    for (unsigned i{}; i < 100; ++i) { ts.emplace_back(do_work, i); }

    for (auto &entry : ts) entry.join();
}

void t2() {
    cout << thread::hardware_concurrency() << endl; // 8
}

int main(int argc, char const *argv[]) {
    // t1();
    t2();
    return 0;
}
/*
31622
1

90

6
21
812
25
7
2

13
14
202819
29
4
30
5
113424

35
36

1518

26
39
38
27
10
42
40

41
45
31
46
23
49
32
50

3348
52
37
54
6261
63
66
4365

6467
68
70
5173
72
57
59
58
60




697471
44
53
76
75
55


79
47
8078
81

56
77

82
85
86
17
83
88
90
84
87
9192
93
89
95
96

97
98
94
99
*/