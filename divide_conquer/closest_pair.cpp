#include "closest_pair.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

static const double INF = std::numeric_limits<double>::infinity();

// Eulidean distane calculator helper
static double dist(const Hub& a, const Hub& b, long long& dist_calls) {
    dist_calls++;
    return std::hypot(a.x - b.x, a.y - b.y);
}


// current best distance update helper
static void update(PairResult& best, const Hub& a, const Hub& b, long long& dist_calls) {
    double d = dist(a, b, dist_calls);
    if (d < best.distance){
        best = {d, a.id, b.id};
    } 
}

static bool by_x(const Hub& a, const Hub& b) { return a.x < b.x || (a.x == b.x && a.y < b.y); }
static bool by_y(const Hub& a, const Hub& b) { return a.y < b.y; }

// Brute force 
PairResult brute_force_closest(const std::vector<Hub>& hubs, long long& dist_calls) {
    PairResult best = {INF, -1, -1};
    for (size_t i = 0; i < hubs.size(); i++)
        for (size_t j = i + 1; j < hubs.size(); j++)
            update(best, hubs[i], hubs[j], dist_calls);
    return best;
}


// Divide and conquer
static void closest_rec(std::vector<Hub>& p, std::vector<Hub>& temp,
                        int low, int high, PairResult& best, long long& dist_calls) {
    int n = high - low + 1;

    // Base case: 3 or lower then sort by y
    if (n <= 3) {
        for (int i = low; i <= high; i++)
            for (int j = i + 1; j <= high; j++)
                update(best, p[i], p[j], dist_calls);
        std::sort(p.begin() + low, p.begin() + high + 1, by_y);
        return;
    }

    // Divide
    int mid = low + (high - low) / 2;
    double mid_x = p[mid].x;

    // Conquer
    closest_rec(p, temp, low, mid, best, dist_calls);
    closest_rec(p, temp, mid + 1, high, best, dist_calls);

    // Combine step 1: merge the two y-sorted halves (linear time)
    std::merge(p.begin() + low, p.begin() + mid + 1,
               p.begin() + mid + 1, p.begin() + high + 1,
               temp.begin() + low, by_y);
    std::copy(temp.begin() + low, temp.begin() + high + 1, p.begin() + low);

    // Combine step 2: check the strip of width 2*d around the dividing line
    std::vector<Hub> strip;
    for (int i = low; i <= high; i++)
        if (std::fabs(p[i].x - mid_x) < best.distance) strip.push_back(p[i]);

    // Each point only needs to be compared with the next few (at most 7)
    // points above it in y order; the loop stops as soon as dy >= d
    for (size_t i = 0; i < strip.size(); i++)
        for (size_t j = i + 1; j < strip.size() && strip[j].y - strip[i].y < best.distance; j++)
            update(best, strip[i], strip[j], dist_calls);
}

PairResult closest_pair(const std::vector<Hub>& hubs, long long& dist_calls) {
    PairResult best = {INF, -1, -1};
    if (hubs.size() < 2) return best;              // edge case: 0 or 1 hub

    std::vector<Hub> p = hubs;                     // copy: do not change caller's data
    std::sort(p.begin(), p.end(), by_x);           // pre-sort once by x: O(n log n)
    std::vector<Hub> temp(p.size());
    closest_rec(p, temp, 0, static_cast<int>(p.size()) - 1, best, dist_calls);
    return best;
}