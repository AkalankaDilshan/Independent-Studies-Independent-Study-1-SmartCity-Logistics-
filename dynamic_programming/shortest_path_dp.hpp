#pragma once
#include <vector>
#include "../common/models.hpp"

struct ShortestPathResult {
    std::vector<long long> dist;   // dist[v] = cheapest cost from source; INF_COST if unreachable
    std::vector<int> pred;         // pred[v] = previous node on the cheapest path; -1 if none
    bool negative_cycle;           // true if a reachable negative cycle exists
    int rounds;                    // how many relaxation rounds were actually needed
    long long relaxations;         // edge checks done (work counter)
};

const long long INF_COST = 1000000000000000000LL;   // 10^18 means "unreachable"

// Bellman-Ford (dynamic programming over "number of edges used").
// V nodes numbered 0..V-1, directed edges. O(V * E) time, O(V) space.
ShortestPathResult bellman_ford(int V, const std::vector<Edge>& edges, int source);

// Rebuild the node list source -> ... -> target from pred[]. Empty if unreachable.
std::vector<int> get_path(const ShortestPathResult& r, int source, int target);

// A two-way road becomes two directed edges.
void add_road(std::vector<Edge>& edges, int a, int b, int cost);