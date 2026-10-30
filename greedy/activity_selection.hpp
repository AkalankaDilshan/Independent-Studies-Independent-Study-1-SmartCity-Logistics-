#pragma once
#include <vector>
#include "../common/models.hpp"

// Greedy "earliest finish time": returns the largest set of non-overlapping
// jobs, in the order they are done. A job may start exactly when the previous
// one ends (end == start is allowed). O(n log n) because of the sort.
std::vector<Job> activity_selection(const std::vector<Job>& jobs);

// Exhaustive check of every subset, O(2^n * n). Only for testing small inputs.
int max_jobs_brute_force(const std::vector<Job>& jobs);