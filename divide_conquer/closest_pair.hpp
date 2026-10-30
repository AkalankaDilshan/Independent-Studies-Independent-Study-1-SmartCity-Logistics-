#pragma once
#include <vector>
#include "../common/models.hpp"

//closes pair search result
struct PairResult
{
    double distance;
    int id1;
    int id2;
};

// brute force search O(n^2)
PairResult brute_force_closest(const std::vector<Hub>& hubs, long long& dist_calls);

// O(n log n): recursive divide & conquer.
PairResult closest_pair(const std::vector<Hub>& hubs, long long& dist_calls);

// dis_call -> its counts of real work distance calculations like comparison previus merge sort
