// for merge sort
#include <cassert>
#include <iostream>
#include <vector>
#include "../divide_conquer/merge_sort.hpp"

//for closest pair
#include <cmath>
#include <random>
#include "../divide_conquer/closest_pair.hpp"

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

int main() {
    test_merge_sort();
    test_closest_pair();
    std::cout << "All tests passed.\n";
    return 0;
}