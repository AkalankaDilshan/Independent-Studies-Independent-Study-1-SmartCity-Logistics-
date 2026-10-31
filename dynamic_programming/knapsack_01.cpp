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


// 2D table
// dp[i][w] is the maximum value we can achieve using the first i packages when the vehicle has a capacity of w kg
KnapsackResult knapsack_2d(const std::vector<Item>& items, int capacity,std::vector<std::vector<int>>* table_out) {
    KnapsackResult res = {0, {}};
    if (capacity <= 0) return res;
    int n = static_cast<int>(items.size());
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));   // row 0 = no items, initate in their

    for (int i = 1; i <= n; i++) {
        int wt = items[i - 1].weight, val = items[i - 1].value;
        for (int w = 0; w <= capacity; w++) {
            dp[i][w] = dp[i - 1][w];                                  // if skip item i
            if (wt <= w)                                              // if take item i
                dp[i][w] = std::max(dp[i][w], val + dp[i - 1][w - wt]);
        }
    }
    res.best_value = dp[n][capacity];

    // Trace back
    for (int i = n, w = capacity; i >= 1; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            res.chosen_ids.push_back(items[i - 1].id);
            w -= items[i - 1].weight;
        }
    }
    std::reverse(res.chosen_ids.begin(), res.chosen_ids.end());
    if (table_out) *table_out = dp;
    return res;
}

// 1D rolling array
int knapsack_1d(const std::vector<Item>& items, int capacity) {
    if (capacity <= 0) return 0;
    std::vector<int> dp(capacity + 1, 0);                            
    for (const Item& it : items) {
        
        for (int w = capacity; w >= it.weight; w--)
            dp[w] = std::max(dp[w], it.value + dp[w - it.weight]);
    }
    return dp[capacity];
}