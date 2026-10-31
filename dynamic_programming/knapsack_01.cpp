#include "knapsack_01.hpp"
#include <algorithm>

// Brute force: O(2^n) 
static int brute_rec(const std::vector<Item>& items, int i, int remaining, long long& calls) {
    calls++;
    if (i == static_cast<int>(items.size()) || remaining == 0) return 0;   
    int skip = brute_rec(items, i + 1, remaining, calls);                  
    int take = 0;
    if (items[i].weight <= remaining)                                      
        take = items[i].value + brute_rec(items, i + 1, remaining - items[i].weight, calls);
    return std::max(skip, take);
}

int knapsack_brute(const std::vector<Item>& items, int capacity, long long& calls) {
    if (capacity <= 0) return 0;
    return brute_rec(items, 0, capacity, calls);
}