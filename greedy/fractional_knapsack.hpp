#pragma once
#include <vector>
#include "../common/models.hpp"

// How much of one item was loaded
struct LoadedPart {
    int id;
    double fraction;   // 1.0 = whole item, 0 < f < 1 = part of the item
};

struct FractionalResult {
    double total_value;
    double total_weight;
    std::vector<LoadedPart> parts;   // in the order they were loaded
};

// Greedy by value/weight ratio (highest first). Items may be split.
// O(n log n) because of the sort.
FractionalResult fractional_knapsack(const std::vector<Item>& items, int capacity);