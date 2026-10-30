#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include "../divide_conquer/merge_sort.hpp"

//closes pair
#include "../divide_conquer/closest_pair.hpp"

//activity selection
#include <algorithm>
#include <functional>
#include "../greedy/activity_selection.hpp"

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

//activity selection
// Generic greedy with a different sorting rule (for comparison only)
static int greedy_by(std::vector<Job> jobs, std::function<bool(const Job&, const Job&)> rule) {
    std::sort(jobs.begin(), jobs.end(), rule);
    std::vector<Job> chosen;
    for (const Job& j : jobs) {
        bool ok = true;
        for (const Job& c : chosen)
            if (j.start < c.end && c.start < j.end) { ok = false; break; }   // overlap
        if (ok) chosen.push_back(j);
    }
    return static_cast<int>(chosen.size());
}

static void demo_greedy_rules() {
    auto earliest_start = [](const Job& a, const Job& b) { return a.start < b.start; };
    auto shortest       = [](const Job& a, const Job& b) { return a.end - a.start < b.end - b.start; };

    struct Case { const char* name; std::vector<Job> jobs; };
    std::vector<Case> cases = {
        {"A: one long early job", {{1, 0, 10}, {2, 1, 2}, {3, 3, 4}}},
        {"B: short job in the middle", {{1, 0, 5}, {2, 4, 7}, {3, 6, 11}}},
    };

    std::cout << "case\t\t\t\tearliest_start\tshortest\tearliest_finish\toptimal\n";
    for (const Case& c : cases) {
        std::cout << c.name << "\t" << greedy_by(c.jobs, earliest_start) << "\t\t"
                  << greedy_by(c.jobs, shortest) << "\t\t"
                  << activity_selection(c.jobs).size() << "\t\t"
                  << max_jobs_brute_force(c.jobs) << "\n";
    }
}

int main() {
    bench_merge_sort();
    std::cout << "\n";
    bench_closest_pair();
    std::cout << "\n";
    demo_greedy_rules();
    return 0;
}