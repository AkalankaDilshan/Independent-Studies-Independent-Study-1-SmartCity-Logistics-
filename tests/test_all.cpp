// for merge sort
#include <cassert>
#include <iostream>
#include <vector>
#include "../divide_conquer/merge_sort.hpp"

//for closest pair
#include <cmath>
#include <random>
#include "../divide_conquer/closest_pair.hpp"

// for activity selection
#include "../greedy/activity_selection.hpp"

//for fractional knapsack
#include "../greedy/fractional_knapsack.hpp"

//huffman
#include <map>
#include <string>
#include "../greedy/huffman.hpp"

//DP 0/1 Knapsack
#include "../dynamic_programming/knapsack_01.hpp"

//DP shortest path
#include <algorithm>
#include "../dynamic_programming/shortest_path_dp.hpp"

static bool is_sorted_by_deadline(const std::vector<Package>& v) {
    for (size_t i = 1; i < v.size(); i++)
        if (v[i - 1].deadline > v[i].deadline) return false;
    return true;
}

static void test_merge_sort() {
    long long c = 0;

    // 1. empty list
    std::vector<Package> empty;
    merge_sort(empty, c);
    assert(empty.empty());

    // 2. one element
    std::vector<Package> one = {{1, "Colombo", 5, 2.0, 1000}};
    merge_sort(one, c);
    assert(one.size() == 1 && one[0].id == 1);

    // 3. already sorted
    std::vector<Package> sorted = {{1,"A",1,1,1},{2,"B",2,1,1},{3,"C",3,1,1}};
    merge_sort(sorted, c);
    assert(is_sorted_by_deadline(sorted));

    // 4. reverse sorted
    std::vector<Package> rev = {{1,"A",9,1,1},{2,"B",5,1,1},{3,"C",1,1,1}};
    merge_sort(rev, c);
    assert(is_sorted_by_deadline(rev) && rev[0].id == 3);

    // 5. duplicate deadlines: stable (original order kept for ties)
    std::vector<Package> dup = {{1,"A",4,1,1},{2,"B",2,1,1},{3,"C",4,1,1},{4,"D",2,1,1}};
    merge_sort(dup, c);
    assert(dup[0].id == 2 && dup[1].id == 4 && dup[2].id == 1 && dup[3].id == 3);

    std::cout << "[PASS] merge_sort: 5 tests\n";
}


///Closest pair testing
static void test_closest_pair() {
    long long c = 0;

    // 1. empty list -> no pair
    std::vector<Hub> empty;
    PairResult r = closest_pair(empty, c);
    assert(std::isinf(r.distance) && r.id1 == -1);

    // 2. one hub -> no pair
    std::vector<Hub> one = {{1, 5, 5}};
    r = closest_pair(one, c);
    assert(std::isinf(r.distance) && r.id1 == -1);

    // 3. two hubs -> that pair (3-4-5 triangle)
    std::vector<Hub> two = {{1, 0, 0}, {2, 3, 4}};
    r = closest_pair(two, c);
    assert(std::fabs(r.distance - 5.0) < 1e-9);

    // 4. duplicate points -> distance 0
    std::vector<Hub> dup = {{1, 0, 0}, {2, 10, 10}, {3, 4, 4}, {4, 10, 10}, {5, 20, 1}};
    r = closest_pair(dup, c);
    assert(r.distance == 0.0);
    assert((r.id1 == 2 && r.id2 == 4) || (r.id1 == 4 && r.id2 == 2));

    // 5. all hubs on one vertical line (same x) -> splitting by x still works
    std::vector<Hub> vertical = {{1, 5, 0}, {2, 5, 10}, {3, 5, 3}, {4, 5, 7}, {5, 5, 20}, {6, 5, 8.5}};
    r = closest_pair(vertical, c);
    // y values 0, 3, 7, 8.5, 10, 20 -> smallest gap is 1.5
    assert(std::fabs(r.distance - 1.5) < 1e-9);

    // 6. closest pair crosses the dividing line (strip must find it)
    std::vector<Hub> cross = {{1, 0, 0}, {2, 1, 50}, {3, 2, 100}, {4, 49.9, 30}, {5, 50.1, 30.1},
                              {6, 98, 0}, {7, 99, 50}, {8, 100, 100}};
    r = closest_pair(cross, c);
    long long c2 = 0;
    assert(std::fabs(r.distance - brute_force_closest(cross, c2).distance) < 1e-9);

    // 7. random validation: D&C must equal brute force every time
    std::mt19937 rng(7);
    std::uniform_real_distribution<double> coord(0, 1000);
    std::uniform_int_distribution<int> size(2, 300);
    for (int t = 0; t < 200; t++) {
        int n = size(rng);
        std::vector<Hub> pts(n);
        for (int i = 0; i < n; i++) pts[i] = {i, coord(rng), coord(rng)};
        long long a = 0, b = 0;
        assert(std::fabs(closest_pair(pts, a).distance - brute_force_closest(pts, b).distance) < 1e-9);
    }

    std::cout << "[PASS] closest_pair: 7 tests (incl. 200 random checks)\n";
}

// activity selection
// true if no two chosen jobs overlap (chosen list is in finish order)
static bool no_overlap(const std::vector<Job>& v) {
    for (size_t i = 1; i < v.size(); i++)
        if (v[i].start < v[i - 1].end) return false;
    return true;
}

static void test_activity_selection() {
    // 1. empty list -> nothing scheduled
    assert(activity_selection({}).empty());

    // 2. one job -> that job
    std::vector<Job> one = {{1, 9, 11}};
    assert(activity_selection(one).size() == 1);

    // 3. all overlapping -> only one can be served
    std::vector<Job> all_overlap = {{1, 1, 10}, {2, 2, 9}, {3, 3, 8}, {4, 4, 7}};
    std::vector<Job> r = activity_selection(all_overlap);
    assert(r.size() == 1 && r[0].id == 4);              // earliest finish = job 4

    // 4. none overlapping -> all are served
    std::vector<Job> none = {{1, 1, 2}, {2, 3, 4}, {3, 5, 6}};
    assert(activity_selection(none).size() == 3);

    // 5. touching windows (end == next start) are compatible
    std::vector<Job> touch = {{1, 8, 10}, {2, 10, 12}, {3, 12, 14}};
    assert(activity_selection(touch).size() == 3);

    // 6. identical windows -> only one of them
    std::vector<Job> same = {{1, 9, 12}, {2, 9, 12}, {3, 9, 12}};
    assert(activity_selection(same).size() == 1);

    // 7. unsorted input with the textbook example (answer = 4)
    std::vector<Job> book = {{1, 5, 9}, {2, 1, 4}, {3, 3, 5}, {4, 0, 6}, {5, 5, 7},
                             {6, 3, 9}, {7, 6, 10}, {8, 8, 11}, {9, 8, 12}, {10, 2, 14},
                             {11, 12, 16}};
    r = activity_selection(book);
    assert(r.size() == 4 && no_overlap(r));

    // 8. random validation: greedy count == brute-force optimum
    std::mt19937 rng(11);
    std::uniform_int_distribution<int> size(0, 12), st(0, 40), len(1, 10);
    for (int t = 0; t < 300; t++) {
        int n = size(rng);
        std::vector<Job> jobs(n);
        for (int i = 0; i < n; i++) { int s = st(rng); jobs[i] = {i, s, s + len(rng)}; }
        std::vector<Job> g = activity_selection(jobs);
        assert(no_overlap(g));
        assert(static_cast<int>(g.size()) == max_jobs_brute_force(jobs));
    }

    std::cout << "[PASS] activity_selection: 8 tests (incl. 300 random checks vs brute force)\n";
}


//Fractional knapsack
static bool near(double a, double b) { return std::fabs(a - b) < 1e-9; }

// 0/1 optimum by trying every subset (small n only) - used as a lower bound
static int best_01_brute(const std::vector<Item>& items, int W) {
    int n = static_cast<int>(items.size()), best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int w = 0, v = 0;
        for (int i = 0; i < n; i++)
            if (mask & (1 << i)) { w += items[i].weight; v += items[i].value; }
        if (w <= W) best = std::max(best, v);
    }
    return best;
}

static void test_fractional_knapsack() {
    // Classic example: W = 50, items (weight, value)
    std::vector<Item> classic = {{1, 10, 60}, {2, 20, 100}, {3, 30, 120}};

    // 1. classic example: all of 1 and 2, then 20/30 of item 3 -> 60 + 100 + 80 = 240
    FractionalResult r = fractional_knapsack(classic, 50);
    assert(near(r.total_value, 240.0) && near(r.total_weight, 50.0));
    assert(r.parts.size() == 3 && r.parts[2].id == 3 && near(r.parts[2].fraction, 2.0 / 3.0));

    // 2. capacity 0 -> nothing loaded
    r = fractional_knapsack(classic, 0);
    assert(r.parts.empty() && near(r.total_value, 0.0));

    // 3. no items -> nothing loaded
    r = fractional_knapsack({}, 50);
    assert(r.parts.empty());

    // 4. capacity larger than total weight -> everything, whole
    r = fractional_knapsack(classic, 1000);
    assert(near(r.total_value, 280.0) && near(r.total_weight, 60.0));
    for (const LoadedPart& p : r.parts) assert(near(p.fraction, 1.0));

    // 5. exact fit -> whole items only, no fraction
    r = fractional_knapsack(classic, 30);                 // items 1 + 2 = 30 kg
    assert(near(r.total_value, 160.0) && r.parts.size() == 2);

    // 6. one item heavier than capacity -> a fraction of it
    std::vector<Item> heavy = {{1, 40, 200}};
    r = fractional_knapsack(heavy, 10);
    assert(near(r.total_value, 50.0) && near(r.parts[0].fraction, 0.25));

    // 7. zero-weight item (e.g. a document) -> taken for free, no divide-by-zero
    std::vector<Item> zero = {{1, 0, 30}, {2, 10, 50}};
    r = fractional_knapsack(zero, 5);
    assert(near(r.total_value, 30.0 + 25.0));

    // 8. random properties (300 cases):
    //    weight <= capacity, fractions in (0,1], at most one fractional item,
    //    and fractional value >= best 0/1 value (splitting can never be worse)
    std::mt19937 rng(5);
    std::uniform_int_distribution<int> size(0, 12), wt(1, 30), val(1, 100), cap(0, 120);
    for (int t = 0; t < 300; t++) {
        int n = size(rng), W = cap(rng);
        std::vector<Item> items(n);
        for (int i = 0; i < n; i++) items[i] = {i, wt(rng), val(rng)};
        r = fractional_knapsack(items, W);
        assert(r.total_weight <= W + 1e-9);
        int fractional_count = 0;
        for (const LoadedPart& p : r.parts) {
            assert(p.fraction > 0 && p.fraction <= 1.0 + 1e-12);
            if (p.fraction < 1.0) fractional_count++;
        }
        assert(fractional_count <= 1);
        assert(r.total_value + 1e-9 >= best_01_brute(items, W));
    }

    std::cout << "[PASS] fractional_knapsack: 8 tests (incl. 300 random property checks)\n";
}

//huffman
// No code may be the beginning of another code (prefix-free property)
static bool is_prefix_free(const std::map<char, std::string>& codes) {
    for (const auto& [c1, s1] : codes)
        for (const auto& [c2, s2] : codes)
            if (c1 != c2 && s2.compare(0, s1.size(), s1) == 0) return false;
    return true;
}

static std::string round_trip(const std::string& text) {
    auto root = build_huffman_tree(count_frequencies(text));
    auto codes = build_codes(root.get());
    return huffman_decode(huffman_encode(text, codes), root.get());
}

static void test_huffman() {
    // 1. textbook frequencies a45 b13 c12 d16 e9 f5 -> optimal cost 224 bits
    std::map<char, int> f = {{'a', 45}, {'b', 13}, {'c', 12}, {'d', 16}, {'e', 9}, {'f', 5}};
    auto root = build_huffman_tree(f);
    auto codes = build_codes(root.get());
    long long cost = 0;
    for (const auto& [c, n] : f) cost += static_cast<long long>(n) * codes[c].size();
    assert(cost == 224);
    assert(codes['a'].size() == 1);                // most frequent -> shortest code
    assert(is_prefix_free(codes));

    // 2. empty text -> empty everything, no crash
    assert(build_huffman_tree({}) == nullptr);
    assert(round_trip("").empty());

    // 3. single distinct character -> code "0", still lossless
    assert(round_trip("AAAAA") == "AAAAA");

    // 4. two distinct characters -> 1 bit each
    auto r2 = build_huffman_tree(count_frequencies("ABABBB"));
    auto c2 = build_codes(r2.get());
    assert(c2['A'].size() == 1 && c2['B'].size() == 1);

    // 5. spaces, digits, punctuation and newlines survive (real manifest content)
    std::string line = "PKG-1001, COLOMBO 03, 4.5 KG\nPKG-1002, NEGOMBO, 0.5 KG\n";
    assert(round_trip(line) == line);

    // 6. random texts: always lossless and prefix-free
    std::mt19937 rng(3);
    std::uniform_int_distribution<int> len(1, 400), ch(32, 126);
    for (int t = 0; t < 200; t++) {
        std::string s(len(rng), ' ');
        for (char& c : s) c = static_cast<char>(ch(rng));
        auto rt = build_huffman_tree(count_frequencies(s));
        auto ct = build_codes(rt.get());
        assert(is_prefix_free(ct));
        assert(huffman_decode(huffman_encode(s, ct), rt.get()) == s);
    }

    std::cout << "[PASS] huffman: 6 tests (incl. 200 random lossless checks)\n";
}

// dP knapsack test case
static void test_knapsack_01() {
    long long calls = 0;

    // 1. Week 1 counter-example: greedy got 160, DP must find 220 (items 2 and 3)
    std::vector<Item> van = {{1, 10, 60}, {2, 20, 100}, {3, 30, 120}};
    KnapsackResult r = knapsack_2d(van, 50);
    assert(r.best_value == 220);
    assert(r.chosen_ids.size() == 2 && r.chosen_ids[0] == 2 && r.chosen_ids[1] == 3);
    assert(knapsack_1d(van, 50) == 220 && knapsack_brute(van, 50, calls) == 220);

    // 2. hand-trace example: W = 5, (2,3) (3,4) (4,5) (5,6) -> 7 (items 1 and 2)
    std::vector<Item> small = {{1, 2, 3}, {2, 3, 4}, {3, 4, 5}, {4, 5, 6}};
    r = knapsack_2d(small, 5);
    assert(r.best_value == 7 && r.chosen_ids.size() == 2);

    // 3. capacity 0 -> nothing
    assert(knapsack_2d(van, 0).best_value == 0 && knapsack_1d(van, 0) == 0);

    // 4. no items -> nothing
    assert(knapsack_2d({}, 50).best_value == 0 && knapsack_1d({}, 50) == 0);

    // 5. every item heavier than the capacity -> nothing
    std::vector<Item> heavy = {{1, 60, 500}, {2, 80, 900}};
    assert(knapsack_2d(heavy, 50).best_value == 0 && knapsack_1d(heavy, 50) == 0);

    // 6. each item used at most once (1D loop direction check):
    //    one item (1 kg, 10), W = 5 -> 10, NOT 50
    std::vector<Item> one = {{1, 1, 10}};
    assert(knapsack_1d(one, 5) == 10 && knapsack_2d(one, 5).best_value == 10);

    // 7. random: brute == 2D == 1D, and the chosen items really fit and add up
    std::mt19937 rng(9);
    std::uniform_int_distribution<int> size(0, 14), wt(1, 25), val(1, 100), cap(0, 80);
    for (int t = 0; t < 300; t++) {
        int n = size(rng), W = cap(rng);
        std::vector<Item> items(n);
        for (int i = 0; i < n; i++) items[i] = {i, wt(rng), val(rng)};
        long long c = 0;
        int b = knapsack_brute(items, W, c);
        KnapsackResult d2 = knapsack_2d(items, W);
        assert(b == d2.best_value && b == knapsack_1d(items, W));
        int w_sum = 0, v_sum = 0;
        for (int id : d2.chosen_ids) { w_sum += items[id].weight; v_sum += items[id].value; }
        assert(w_sum <= W && v_sum == d2.best_value);
    }

    std::cout << "[PASS] knapsack_01: 7 tests (incl. 300 random checks: brute == 2D == 1D)\n";
}

//DP shprtest path test cases

// All-pairs check with Floyd-Warshall O(V^3) - independent ground truth
static std::vector<long long> floyd_from(int V, const std::vector<Edge>& edges, int src) {
    std::vector<std::vector<long long>> d(V, std::vector<long long>(V, INF_COST));
    for (int i = 0; i < V; i++) d[i][i] = 0;
    for (const Edge& e : edges) d[e.from][e.to] = std::min(d[e.from][e.to], static_cast<long long>(e.cost));
    for (int k = 0; k < V; k++)
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                if (d[i][k] != INF_COST && d[k][j] != INF_COST && d[i][k] + d[k][j] < d[i][j])
                    d[i][j] = d[i][k] + d[k][j];
    return d[src];
}

static void test_bellman_ford() {
    // 1. single node -> distance 0 to itself, path is just [0]
    ShortestPathResult r = bellman_ford(1, {}, 0);
    assert(r.dist[0] == 0 && !r.negative_cycle && get_path(r, 0, 0).size() == 1);

    // 2. unreachable node -> INF and empty path
    std::vector<Edge> e2 = {{0, 1, 5}};                     // node 2 has no roads
    r = bellman_ford(3, e2, 0);
    assert(r.dist[2] == INF_COST && get_path(r, 0, 2).empty());

    // 3. two-way roads with cycles: cheapest is 0-1-2-3 (1+2+1 = 4), not direct 0-3 (10)
    std::vector<Edge> city;
    add_road(city, 0, 1, 1); add_road(city, 1, 2, 2); add_road(city, 2, 3, 1);
    add_road(city, 0, 3, 10); add_road(city, 1, 3, 6);
    r = bellman_ford(4, city, 0);
    assert(r.dist[3] == 4 && !r.negative_cycle);
    std::vector<int> p = get_path(r, 0, 3);
    assert((p == std::vector<int>{0, 1, 2, 3}));

    // 4. negative edge, no cycle (e.g. a toll refund on a one-way road):
    //    A->B 4, A->C 5, C->B -3  -> B = 2 via C (Dijkstra would wrongly keep 4)
    std::vector<Edge> neg = {{0, 1, 4}, {0, 2, 5}, {2, 1, -3}};
    r = bellman_ford(3, neg, 0);
    assert(r.dist[1] == 2 && !r.negative_cycle);
    assert((get_path(r, 0, 1) == std::vector<int>{0, 2, 1}));

    // 5. negative cycle reachable from source -> detected, no path returned
    std::vector<Edge> cyc = {{0, 1, 1}, {1, 2, 2}, {2, 1, -4}, {2, 3, 1}};
    r = bellman_ford(4, cyc, 0);
    assert(r.negative_cycle && get_path(r, 0, 3).empty());

    // 6. early exit: a 5-node chain listed in path order settles in 1 round (+1 to confirm)
    std::vector<Edge> chain = {{0, 1, 1}, {1, 2, 1}, {2, 3, 1}, {3, 4, 1}};
    r = bellman_ford(5, chain, 0);
    assert(r.dist[4] == 4 && r.rounds == 2);               // far fewer than V-1 = 4

    // 7. invalid source -> nothing reachable, no crash
    r = bellman_ford(3, e2, 7);
    assert(r.dist[0] == INF_COST);

    // 8. random directed graphs (weights 0..20) vs Floyd-Warshall
    std::mt19937 rng(21);
    std::uniform_int_distribution<int> vs(1, 15), cost(0, 20);
    for (int t = 0; t < 200; t++) {
        int V = vs(rng);
        std::uniform_int_distribution<int> node(0, V - 1);
        std::vector<Edge> edges;
        int E = V * 2;
        for (int i = 0; i < E; i++) edges.push_back({node(rng), node(rng), cost(rng)});
        int src = node(rng);
        r = bellman_ford(V, edges, src);
        assert(r.dist == floyd_from(V, edges, src));
        for (int v = 0; v < V; v++) {                        // every path really costs dist[v]
            std::vector<int> path = get_path(r, src, v);
            if (r.dist[v] == INF_COST) { assert(path.empty()); continue; }
            assert(path.front() == src && path.back() == v);
            long long sum = 0;                               // add up the cheapest road per step
            for (size_t i = 1; i < path.size(); i++) {
                long long best = INF_COST;
                for (const Edge& e : edges)
                    if (e.from == path[i - 1] && e.to == path[i]) best = std::min(best, static_cast<long long>(e.cost));
                sum += best;
            }
            assert(sum == r.dist[v]);
        }
    }

    std::cout << "[PASS] bellman_ford: 8 tests (incl. 200 random checks vs Floyd-Warshall)\n";
}

int main() {
    test_merge_sort();
    test_closest_pair();
    test_activity_selection();
    test_fractional_knapsack();
    test_huffman();
    test_knapsack_01();
    test_bellman_ford();
    std::cout << "All tests passed.\n";
    return 0;
}