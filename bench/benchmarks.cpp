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

//fractional knapsack
#include "../greedy/fractional_knapsack.hpp"

//huffman
#include <sstream>
#include "../greedy/huffman.hpp"

//DP knapsack
#include <iomanip>
#include "../dynamic_programming/knapsack_01.hpp"

//DP shortest path
#include <string>
#include "../dynamic_programming/shortest_path_dp.hpp"

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

//fractional knapsack
// Greedy by ratio, but WHOLE items only (what happens if you reuse the
// fractional idea for indivisible cargo)
static int greedy_whole_items(std::vector<Item> items, int W) {
    std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return static_cast<long long>(a.value) * b.weight >
               static_cast<long long>(b.value) * a.weight;
    });
    int v = 0;
    for (const Item& it : items)
        if (it.weight <= W) { W -= it.weight; v += it.value; }
    return v;
}

// True 0/1 optimum by trying every subset (small n only)
static int brute_01(const std::vector<Item>& items, int W) {
    int n = static_cast<int>(items.size()), best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int w = 0, v = 0;
        for (int i = 0; i < n; i++)
            if (mask & (1 << i)) { w += items[i].weight; v += items[i].value; }
        if (w <= W) best = std::max(best, v);
    }
    return best;
}

static void demo_knapsack_greedy() {
    std::vector<Item> items = {{1, 10, 60}, {2, 20, 100}, {3, 30, 120}};   // ratios 6, 5, 4
    int W = 50;
    FractionalResult fr = fractional_knapsack(items, W);
    std::cout << "Van capacity " << W << " kg, items (kg, LKR): (10,60) (20,100) (30,120)\n";
    std::cout << "Fractional greedy (items can be split) : " << fr.total_value << "\n";
    std::cout << "Ratio greedy, whole items only (0/1)   : " << greedy_whole_items(items, W) << "\n";
    std::cout << "True 0/1 optimum (brute force)         : " << brute_01(items, W) << "\n";
}

//huffman
// Show special characters readably in the code table
static std::string show(char c) {
    if (c == ' ') return "' '";
    if (c == '\n') return "\\n";
    return std::string(1, c);
}

static void demo_huffman() {
    std::ifstream in("data/sample_manifest.txt");
    if (!in) { std::cout << "Huffman: data/sample_manifest.txt not found\n"; return; }
    std::stringstream ss;
    ss << in.rdbuf();
    std::string text = ss.str();

    auto freq  = count_frequencies(text);
    auto root  = build_huffman_tree(freq);
    auto codes = build_codes(root.get());
    std::string bits    = huffman_encode(text, codes);
    std::string decoded = huffman_decode(bits, root.get());

    size_t n = text.size(), k = freq.size();
    size_t ascii_bits = 8 * n;
    size_t fixed_len  = static_cast<size_t>(std::ceil(std::log2(static_cast<double>(k))));
    size_t fixed_bits = fixed_len * n;
    double ratio = static_cast<double>(bits.size()) / ascii_bits;

    std::cout << "Huffman on data/sample_manifest.txt\n";
    std::cout << "Characters: " << n << ", distinct: " << k << "\n";
    std::cout << "Plain ASCII (8 bits/char)     : " << ascii_bits << " bits\n";
    std::cout << "Fixed-length (" << fixed_len << " bits/char)    : " << fixed_bits << " bits\n";
    std::cout << "Huffman                       : " << bits.size() << " bits\n";
    std::cout << "Compression ratio (vs ASCII)  : " << ratio << "  (" << (1 - ratio) * 100 << "% smaller)\n";
    std::cout << "Average bits per character    : " << static_cast<double>(bits.size()) / n << "\n";
    std::cout << "Lossless (decode == original) : " << (decoded == text ? "yes" : "NO") << "\n";

    // Code table, most frequent first -> results/huffman_codes.csv
    std::vector<std::pair<char, int>> rows(freq.begin(), freq.end());
    std::sort(rows.begin(), rows.end(), [](auto& a, auto& b) { return a.second > b.second; });
    std::ofstream out("results/huffman_codes.csv");
    out << "char,frequency,code,code_length\n";
    std::cout << "Top 8 codes (char freq code):\n";
    for (size_t i = 0; i < rows.size(); i++) {
        char c = rows[i].first;
        out << "\"" << show(c) << "\"," << rows[i].second << "," << codes[c] << "," << codes[c].size() << "\n";
        if (i < 8) std::cout << "  " << show(c) << "\t" << rows[i].second << "\t" << codes[c] << "\n";
    }
}

//knapsack 0/1

// Prints the filled DP table for the hand-trace example (for the write-up)
static void print_knapsack_table() {
    std::vector<Item> items = {{1, 2, 3}, {2, 3, 4}, {3, 4, 5}, {4, 5, 6}};
    int W = 5;
    std::vector<std::vector<int>> dp;
    KnapsackResult r = knapsack_2d(items, W, &dp);

    std::cout << "0/1 knapsack DP table, W = " << W << ", items (w,v): (2,3) (3,4) (4,5) (5,6)\n";
    std::cout << "i \\ w ";
    for (int w = 0; w <= W; w++) std::cout << std::setw(4) << w;
    std::cout << "\n";
    for (size_t i = 0; i < dp.size(); i++) {
        std::cout << std::setw(5) << i << " ";
        for (int v : dp[i]) std::cout << std::setw(4) << v;
        std::cout << "\n";
    }
    std::cout << "Best value " << r.best_value << ", items:";
    for (int id : r.chosen_ids) std::cout << " " << id;
    std::cout << "\n\n";
}

static void bench_knapsack() {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> wt(1, 20), val(1, 100);
    std::ofstream out("results/knapsack.csv");
    out << "n,W,brute_calls,brute_ms,dp2d_ms,dp1d_ms,same_answer\n";
    std::cout << "n\tW\tbrute calls\tbrute ms\t2D ms\t1D ms\tsame\n";
    for (int n : {10, 15, 20, 25}) {
        std::vector<Item> items(n);
        int total = 0;
        for (int i = 0; i < n; i++) { items[i] = {i, wt(rng), val(rng)}; total += items[i].weight; }
        int W = total / 2;                 // hard case: about half the items fit

        long long calls = 0;
        auto t0 = std::chrono::steady_clock::now();
        int b = knapsack_brute(items, W, calls);
        auto t1 = std::chrono::steady_clock::now();
        int d2 = knapsack_2d(items, W).best_value;
        auto t2 = std::chrono::steady_clock::now();
        int d1 = knapsack_1d(items, W);
        auto t3 = std::chrono::steady_clock::now();

        auto ms = [](auto a, auto b) { return std::chrono::duration<double, std::milli>(b - a).count(); };
        bool same = (b == d2 && d2 == d1);
        out << n << "," << W << "," << calls << "," << ms(t0, t1) << "," << ms(t1, t2) << ","
            << ms(t2, t3) << "," << (same ? "yes" : "no") << "\n";
        std::cout << n << "\t" << W << "\t" << calls << "\t\t" << ms(t0, t1) << "\t" << ms(t1, t2) << "\t"
                  << ms(t2, t3) << "\t" << (same ? "yes" : "no") << "\n";
    }

    // Memory: table cells x sizeof(int). 2D = (n+1)(W+1), 1D = (W+1)
    std::ofstream mem("results/knapsack_memory.csv");
    mem << "n,W,bytes_2d,bytes_1d,saving_factor\n";
    std::cout << "\nn\tW\t2D bytes\t1D bytes\tsaving\n";
    for (auto [n, cap] : std::vector<std::pair<long long, long long>>{{25, 50}, {100, 1000}, {1000, 10000}}) {
        long long b2 = (n + 1) * (cap + 1) * static_cast<long long>(sizeof(int));
        long long b1 = (cap + 1) * static_cast<long long>(sizeof(int));
        mem << n << "," << cap << "," << b2 << "," << b1 << "," << b2 / b1 << "\n";
        std::cout << n << "\t" << cap << "\t" << b2 << "\t\t" << b1 << "\t\t" << b2 / b1 << "x\n";
    }
}


//Shortest path add the demo city
// SmartCity road map: two-way roads, cost = travel minutes
static const std::vector<std::string> CITY = {
    "Negombo Depot", "Katunayake", "Seeduwa", "Ja-Ela", "Kandana", "Wattala",
    "Kelaniya", "Colombo 15", "Colombo 10", "Colombo 07", "Colombo 03", "Kadawatha"};

static std::vector<Edge> city_roads() {
    std::vector<Edge> e;
    add_road(e, 0, 1, 15); add_road(e, 1, 2, 10); add_road(e, 2, 3, 8);  add_road(e, 0, 3, 30);
    add_road(e, 3, 4, 7);  add_road(e, 4, 5, 9);  add_road(e, 3, 11, 20); add_road(e, 11, 6, 12);
    add_road(e, 5, 6, 10); add_road(e, 5, 7, 8);  add_road(e, 7, 8, 9);  add_road(e, 6, 8, 15);
    add_road(e, 8, 9, 10); add_road(e, 9, 10, 6); add_road(e, 8, 10, 18); add_road(e, 4, 11, 14);
    return e;
}

// Routes from the depot to every area
static void demo_city_routes() {
    std::vector<Edge> roads = city_roads();
    int V = static_cast<int>(CITY.size());
    ShortestPathResult r = bellman_ford(V, roads, 0);
    std::cout << "Bellman-Ford from " << CITY[0] << " (" << V << " nodes, " << roads.size()
              << " directed edges, " << r.rounds << " rounds, negative cycle: "
              << (r.negative_cycle ? "yes" : "no") << ")\n";
    for (int v = 1; v < V; v++) {
        std::cout << "  " << std::setw(11) << std::left << CITY[v] << std::right
                  << std::setw(4) << r.dist[v] << " min  ";
        std::vector<int> p = get_path(r, 0, v);
        for (size_t i = 0; i < p.size(); i++) std::cout << (i ? " -> " : "") << CITY[p[i]];
        std::cout << "\n";
    }
}

// Textbook DP table: D[k][v] = cheapest cost to v using at most k edges.
// D[k][v] = min(D[k-1][v], min over edges (u,v) of D[k-1][u] + cost)
static void print_bf_dp_table() {
    // small graph for a hand trace: S=0, A=1, B=2, C=3, T=4 (one-way, one negative edge)
    std::vector<std::string> name = {"S", "A", "B", "C", "T"};
    std::vector<Edge> e = {{0, 1, 6}, {0, 2, 7}, {1, 3, 5}, {2, 3, -3}, {1, 4, -4}, {3, 4, 2}, {2, 1, 8}};
    int V = 5;
    std::vector<std::vector<long long>> D(V, std::vector<long long>(V, INF_COST));
    D[0][0] = 0;
    for (int k = 1; k < V; k++) {
        D[k] = D[k - 1];
        for (const Edge& ed : e)
            if (D[k - 1][ed.from] != INF_COST)
                D[k][ed.to] = std::min(D[k][ed.to], D[k - 1][ed.from] + ed.cost);
    }
    std::cout << "\nBellman-Ford DP table (edges: S->A 6, S->B 7, A->C 5, B->C -3, A->T -4, C->T 2, B->A 8)\n";
    std::cout << "k \\ v";
    for (auto& n : name) std::cout << std::setw(5) << n;
    std::cout << "\n";
    for (int k = 0; k < V; k++) {
        std::cout << std::setw(5) << k;
        for (long long d : D[k]) std::cout << std::setw(5) << (d == INF_COST ? std::string("inf") : std::to_string(d));
        std::cout << "\n";
    }
    ShortestPathResult r = bellman_ford(V, e, 0);
    std::cout << "bellman_ford() result :";
    for (long long d : r.dist) std::cout << std::setw(5) << d;
    std::cout << "   (rounds used: " << r.rounds << " of " << V - 1 << ")\n";
}

// Scaling on random connected road networks
static void bench_bellman_ford() {
    std::mt19937 rng(42);
    std::ofstream out("results/bellman_ford.csv");
    out << "V,E_directed,rounds,relaxations,worst_case_VxE,time_ms\n";
    std::cout << "\nV\tE\trounds\trelaxations\tworst V*E\ttime ms\n";
    for (int V : {100, 1000, 5000}) {
        std::uniform_int_distribution<int> node(0, V - 1), cost(1, 30);
        std::vector<Edge> edges;
        for (int v = 1; v < V; v++) add_road(edges, v - 1, v, cost(rng));   // a chain keeps it connected
        for (int i = 0; i < 3 * V; i++) add_road(edges, node(rng), node(rng), cost(rng));
        std::shuffle(edges.begin(), edges.end(), rng);

        auto t0 = std::chrono::steady_clock::now();
        ShortestPathResult r = bellman_ford(V, edges, 0);
        auto t1 = std::chrono::steady_clock::now();
        long long E = static_cast<long long>(edges.size());
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        out << V << "," << E << "," << r.rounds << "," << r.relaxations << "," << (V - 1) * E << "," << ms << "\n";
        std::cout << V << "\t" << E << "\t" << r.rounds << "\t" << r.relaxations << "\t\t"
                  << (V - 1) * E << "\t" << ms << "\n";
    }
}


int main() {
    bench_merge_sort();
    std::cout << "\n";
    bench_closest_pair();
    std::cout << "\n";
    demo_greedy_rules();
    std::cout << "\n";
    demo_knapsack_greedy();
    std::cout << "\n";
    demo_huffman();
    std::cout << "\n";
    std::cout << "\n";
    print_knapsack_table();
    bench_knapsack();
    std::cout << "\n";
    demo_city_routes();
    print_bf_dp_table();
    bench_bellman_ford();
    return 0;
}