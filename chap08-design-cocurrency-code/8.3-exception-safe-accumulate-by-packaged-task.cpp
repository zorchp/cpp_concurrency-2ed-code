#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <future>
#include <iomanip>
#include <chrono>
#include <numeric>

using namespace std;
using namespace chrono;

template <typename Iterator, typename T>
struct accmuluate_block {
    T operator()(Iterator first, Iterator last) {
        return accumulate(first, last, T());
    }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    // 设置常量
    unsigned long const length = distance(first, last);
    if (!length) return init; // 如果计算区间为空, 返回初值

    unsigned long const min_per_thread = 25; // 每一个线程计算的数量
    unsigned long const max_threads =        // 最大线程数
        (length + min_per_thread - 1) / min_per_thread;
    unsigned long const hardeare_threads = thread::hardware_concurrency(); // 8
    unsigned long const num_threads = // 实际线程数
        min(hardeare_threads != 0 ? hardeare_threads : 2, max_threads);
    unsigned long const block_size = length / num_threads;

    // 设置线程存储, 存放线程
    vector<std::future<T>> futures(num_threads - 1);
    vector<std::thread> threads(num_threads - 1);

    Iterator block_start = first;

    for (unsigned long i{}; i < num_threads - 1; ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task<T(Iterator, Iterator)> task(
            accmuluate_block<Iterator, T>{});
        futures[i] = task.get_future();
        threads[i] = thread(std::move(task), block_start, block_end);
        block_start = block_end;
    }
    T last_result = accmuluate_block<Iterator, T>()(block_start, last);

    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));
    T result = init;
    for (unsigned long i{}; i < (num_threads - 1); ++i)
        result += futures[i].get();
    result += last_result;
    return result;
}


// vector<int> get_vec() {
//     vector<int> v;
//     for (int i{}; i < 10000000; ++i) v.emplace_back(i);
//     return v;
// }

vector<double> get_vec() {
    vector<double> v;
    std::ranlux48 gen(1);
    std::uniform_real_distribution<> dis(1, 100);
    for (int i{}; i < 1000000; ++i) v.emplace_back(dis(gen));
    return v;
}


void t1() {
    auto v = get_vec();
    auto start = std::chrono::system_clock::now();

    auto ans = parallel_accumulate(v.begin(), v.end(), 0.0);
    // auto ans = accumulate(v.begin(), v.end(), 0.0);

    auto end = std::chrono::system_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(end - start);
    cout << "Time spent: " // << std::fixed << std::setprecision(5)
         << double(duration.count()) * std::chrono::microseconds::period::num /
                std::chrono::microseconds::period::den
         << "s" << endl;

    cout << ans << endl;
    // with parallel:
    // Time spent: 0.014697s
    // -2014260032

    // without parallel:
    // Time spent: 0.083763s
    // -2014260032
}

void t2() {
    auto v = get_vec();
    parallel_accumulate(v.begin(), v.end(), 0.0);
}


int main(int argc, char const* argv[]) {
    t1();
    // t2();
    return 0;
}
