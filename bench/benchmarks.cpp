#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include "../divide_conquer/merge_sort.hpp"

//closes pair
#include "../divide_conquer/closest_pair.hpp"

static void bench_merge_sort() {
    std::mt19937 rng(42);                                
    std::uniform_int_distribution<int> deadline(1, 1000000);
    std::ofstream out("results/merge_sort.csv");
    out << "n,comparisons,n_log2_n,ratio,time_ms\n";
    std::cout << "n\tcomparisons\tn*log2(n)\tratio\ttime_ms\n";

    for (int n : {1000, 5000, 10000, 50000, 100000}) {
        std::vector<Package> v(n);
        for (int i = 0; i < n; i++) v[i] = {i, "X", deadline(rng), 1.0, 1.0};

        long long comps = 0;
        auto t0 = std::chrono::steady_clock::now();
        merge_sort(v, comps);
        auto t1 = std::chrono::steady_clock::now();

        double nlogn = n * std::log2(n);
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        out << n << "," << comps << "," << nlogn << "," << comps / nlogn << "," << ms << "\n";
        std::cout << n << "\t" << comps << "\t" << (long long)nlogn << "\t"
                  << comps / nlogn << "\t" << ms << "\n";
    }
}

//closest pair bench mark
static void bench_closest_pair() {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> coord(0.0, 10000.0);   // city map 10 km x 10 km (metres)
    std::ofstream out("results/closest_pair.csv");
    out << "n,bf_dist_calls,dc_dist_calls,bf_time_ms,dc_time_ms,speedup,same_answer\n";
    std::cout << "n\tBF calls\tD&C calls\tBF ms\tD&C ms\tspeedup\tsame\n";

    for (int n : {100, 1000, 10000}) {
        std::vector<Hub> hubs(n);
        for (int i = 0; i < n; i++) hubs[i] = {i, coord(rng), coord(rng)};

        long long bf_calls = 0, dc_calls = 0;
        auto t0 = std::chrono::steady_clock::now();
        PairResult bf = brute_force_closest(hubs, bf_calls);
        auto t1 = std::chrono::steady_clock::now();
        PairResult dc = closest_pair(hubs, dc_calls);
        auto t2 = std::chrono::steady_clock::now();

        double bf_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        double dc_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
        bool same = bf.distance == dc.distance;

        out << n << "," << bf_calls << "," << dc_calls << "," << bf_ms << "," << dc_ms << ","
            << bf_ms / dc_ms << "," << (same ? "yes" : "no") << "\n";
        std::cout << n << "\t" << bf_calls << "\t" << dc_calls << "\t" << bf_ms << "\t"
                  << dc_ms << "\t" << bf_ms / dc_ms << "\t" << (same ? "yes" : "no") << "\n";
    }
}


int main() {
    bench_merge_sort();
    std::cout << "\n";
    bench_closest_pair();
    return 0;
}