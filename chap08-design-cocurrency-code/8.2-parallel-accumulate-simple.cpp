#include <iostream>
#include <thread>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <numeric>

using namespace std;

template <typename Iterator, typename T>
struct accmuluate_block {
    void operator()(Iterator first, Iterator last, T& result) {
        result = accumulate(first, last, result);
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
    // 存放计算结果,
    vector<T> results(num_threads);

    // 设置线程存储, 存放线程
    vector<thread> threads(num_threads - 1);
    Iterator block_start = first;

    for (unsigned long i{}; i < num_threads - 1; ++i) {
        Iterator block_end = block_start;
        advance(block_end, block_size);
        threads[i] = thread(accmuluate_block<Iterator, T>(), block_start,
                            block_end, ref(results[i])); // 这里使用ref适配器
        block_start = block_end;
    }
    accmuluate_block<Iterator, T>()(block_start, last,
                                    results[num_threads - 1]);

    for (auto& entry : threads) entry.join();
    // 汇总每一个线程分块的结果, 累加得到最终结果, 所以结果需要满足结合律
    // (double/float不满足, 所以可能与串行版accumulate结果有出入)
    return accumulate(results.begin(), results.end(), init);
}


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
    cout << "Time spent: " << std::fixed << std::setprecision(5)
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


int main(int argc, char const* argv[]) {
    t1();
    return 0;
}
