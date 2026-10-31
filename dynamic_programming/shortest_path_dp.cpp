#include "shortest_path_dp.hpp"
#include <algorithm>

ShortestPathResult bellman_ford(int V, const std::vector<Edge>& edges, int source) {
    ShortestPathResult r;
    r.dist.assign(V, INF_COST);
    r.pred.assign(V, -1);
    r.negative_cycle = false;
    r.rounds = 0;
    r.relaxations = 0;
    if (V <= 0 || source < 0 || source >= V) return r;      // invalid input
    r.dist[source] = 0;                                      // base case: 0 edges

    // Round k: after it, dist[v] is at most the cheapest cost of any path
    // that uses k edges or fewer. A shortest simple path has at most V-1 edges.
    for (int k = 1; k <= V - 1; k++) {
        bool changed = false;
        r.rounds++;
        for (const Edge& e : edges) {
            r.relaxations++;
            if (r.dist[e.from] == INF_COST) continue;        // 'from' not reached yet
            long long candidate = r.dist[e.from] + e.cost;
            if (candidate < r.dist[e.to]) {                  // relax: found a cheaper way
                r.dist[e.to] = candidate;
                r.pred[e.to] = e.from;
                changed = true;
            }
        }
        if (!changed) break;                                 // early exit: answers are final
    }

    // Extra round: if anything still improves, a negative cycle is reachable
    for (const Edge& e : edges) {
        if (r.dist[e.from] != INF_COST && r.dist[e.from] + e.cost < r.dist[e.to]) {
            r.negative_cycle = true;
            break;
        }
    }
    return r;
}

std::vector<int> get_path(const ShortestPathResult& r, int source, int target) {
    std::vector<int> path;
    if (r.negative_cycle || target < 0 || target >= static_cast<int>(r.dist.size()) ||
        r.dist[target] == INF_COST) return path;             // no valid path
    for (int v = target; v != -1; v = r.pred[v]) {
        path.push_back(v);
        if (v == source) break;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void add_road(std::vector<Edge>& edges, int a, int b, int cost) {
    edges.push_back({a, b, cost});
    edges.push_back({b, a, cost});
}