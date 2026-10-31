#pragma once
#include <vector>
#include "../common/models.hpp"

struct KnapsackResult {
    int best_value;
    std::vector<int> chosen_ids;     // which items are loaded, this one only for 2D version algorithem
};

// Brute force
int knapsack_brute(const std::vector<Item>& items, int capacity, long long& calls);

// Bottom-up DP with a full 2D
KnapsackResult knapsack_2d(const std::vector<Item>& items, int capacity,
                           std::vector<std::vector<int>>* table_out = nullptr);

int knapsack_1d(const std::vector<Item>& items, int capacity);