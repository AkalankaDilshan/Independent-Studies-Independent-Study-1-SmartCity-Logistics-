#include "fractional_knapsack.hpp"
#include <algorithm>

// a has a better (higher) ratio than b?  Compare v_a/w_a > v_b/w_b
//value_a / weight_a  >  value_b / weight_b
// using cross-multiplication: no division, no rounding, no divide-by-zero.
static bool better_ratio(const Item& a, const Item& b) {
    return static_cast<long long>(a.value) * b.weight > static_cast<long long>(b.value) * a.weight;
}

FractionalResult fractional_knapsack(const std::vector<Item>& items, int capacity) {
    FractionalResult res = {0.0, 0.0, {}};
    if (capacity <= 0) return res;                       // edge case: no space

    std::vector<Item> sorted = items;                    // copy: keep caller's data
    std::stable_sort(sorted.begin(), sorted.end(), better_ratio);

    double remaining = capacity;
    for (const Item& it : sorted) {
        if (remaining <= 0) break;                       // van is full
        if (it.weight <= remaining) {                    // whole item fits
            res.parts.push_back({it.id, 1.0});
            res.total_value  += it.value;
            res.total_weight += it.weight;
            remaining        -= it.weight;
        } else {                                         // take only a fraction
            double f = remaining / it.weight;
            res.parts.push_back({it.id, f});
            res.total_value  += f * it.value;
            res.total_weight += remaining;
            remaining = 0;
        }
    }
    return res;
}