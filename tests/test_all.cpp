#include <cassert>
#include <iostream>
#include <vector>
#include "../divide_conquer/merge_sort.hpp"

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

int main() {
    test_merge_sort();
    std::cout << "All tests passed.\n";
    return 0;
}