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