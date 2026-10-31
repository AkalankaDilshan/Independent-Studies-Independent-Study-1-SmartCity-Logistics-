// SmartCity Logistics - integrated command-line dashboard
// Build: make smartcity     Run: ./smartcity  |  ./smartcity --demo  |  ./smartcity --data <folder>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/csv_loader.hpp"
#include "divide_conquer/merge_sort.hpp"
#include "divide_conquer/closest_pair.hpp"
#include "greedy/activity_selection.hpp"
#include "greedy/fractional_knapsack.hpp"
#include "greedy/huffman.hpp"
#include "dynamic_programming/knapsack_01.hpp"
#include "dynamic_programming/shortest_path_dp.hpp"

// ---------------- input helpers ----------------

// Reads a whole number between lo and hi. Empty input returns 'fallback'.
// Keeps asking on bad input; exits cleanly if input ends (Ctrl+D / piped file).
static int read_int(const std::string& prompt, int lo, int hi, int fallback) {
    while (true) {
        std::cout << prompt << " [" << lo << "-" << hi << ", Enter = " << fallback << "]: ";
        std::string line;
        if (!std::getline(std::cin, line)) { std::cout << "\nInput ended. Goodbye.\n"; std::exit(0); }
        if (line.find_first_not_of(" \t\r") == std::string::npos) return fallback;
        try {
            size_t pos;
            int v = std::stoi(line, &pos);
            if (line.find_first_not_of(" \t\r", pos) == std::string::npos && v >= lo && v <= hi) return v;
        } catch (...) {}
        std::cout << "  Please enter a whole number from " << lo << " to " << hi << ".\n";
    }
}

static void title(const std::string& t, const std::string& paradigm) {
    std::cout << "\n=== " << t << "  [" << paradigm << "] ===\n";
}

// ---------------- 1. Divide & Conquer: merge sort ----------------
static void show_sorted_packages(const CityData& d) {
    title("1. Warehouse queue sorted by deadline", "Divide & Conquer - merge sort");
    std::vector<Package> v = d.packages;                       // keep original order in d
    long long comparisons = 0;
    merge_sort(v, comparisons);
    std::cout << std::left << std::setw(6) << "Order" << std::setw(8) << "Pkg" << std::setw(14) << "Destination"
              << std::setw(10) << "Deadline" << "Weight\n" << std::right;
    for (size_t i = 0; i < v.size(); i++)
        std::cout << std::left << std::setw(6) << i + 1 << std::setw(8) << v[i].id << std::setw(14)
                  << v[i].destination << std::setw(10) << format_time(v[i].deadline) << std::right
                  << v[i].weight << " kg\n";
    double n = static_cast<double>(v.size());
    std::cout << "Comparisons: " << comparisons << "  (n log2 n = " << std::fixed << std::setprecision(0)
              << n * std::log2(std::max(n, 1.0)) << ")\n" << std::defaultfloat;
}

// ---------------- 2. Divide & Conquer: closest pair ----------------
static void show_closest_hubs(const CityData& d) {
    title("2. Closest pair of hubs (merge candidates)", "Divide & Conquer - closest pair");
    long long dc_calls = 0, bf_calls = 0;
    PairResult r = closest_pair(d.hubs, dc_calls);
    PairResult b = brute_force_closest(d.hubs, bf_calls);
    if (r.id1 < 0) { std::cout << "Need at least 2 hubs.\n"; return; }
    std::cout << "Closest hubs : " << d.hub_names[r.id1] << "  <->  " << d.hub_names[r.id2] << "\n";
    std::cout << "Distance     : " << std::fixed << std::setprecision(0) << r.distance << " m ("
              << std::setprecision(2) << r.distance / 1000 << " km)\n" << std::defaultfloat;
    std::cout << "Distance checks: D&C " << dc_calls << " vs brute force " << bf_calls
              << "  | same answer: " << (r.distance == b.distance ? "yes" : "NO") << "\n";
}

// ---------------- 3. Greedy: activity selection ----------------
static void show_driver_schedule(const CityData& d) {
    title("3. One driver's schedule (most deliveries)", "Greedy - earliest finish time");
    std::vector<Job> chosen = activity_selection(d.jobs);
    for (const Job& j : chosen)
        std::cout << "  " << format_time(j.start) << "-" << format_time(j.end) << "  " << d.job_names[j.id] << "\n";
    std::cout << "Deliveries served: " << chosen.size() << " of " << d.jobs.size() << " requests";
    if (d.jobs.size() <= 20)                                   // brute force is only feasible for small n
        std::cout << "  | optimal (brute-force check): "
                  << (static_cast<int>(chosen.size()) == max_jobs_brute_force(d.jobs) ? "yes" : "NO");
    std::cout << "\n";
}

// ---------------- 4. Greedy: fractional knapsack ----------------
static void show_fractional(const CityData& d, int capacity) {
    title("4. Van loading, cargo may be split (capacity " + std::to_string(capacity) + " kg)",
          "Greedy - value/weight ratio");
    FractionalResult r = fractional_knapsack(d.cargo, capacity);
    for (const LoadedPart& p : r.parts)
        std::cout << "  " << std::left << std::setw(22) << d.cargo_names[p.id] << std::right << std::fixed
                  << std::setprecision(0) << std::setw(5) << p.fraction * 100 << "% of "
                  << d.cargo[p.id].weight << " kg\n" << std::defaultfloat;
    std::cout << std::fixed << std::setprecision(2) << "Total: " << r.total_weight << " kg, LKR "
              << r.total_value << "\n" << std::defaultfloat;
}

// ---------------- 5. Greedy: Huffman ----------------
static void show_compression(const CityData& d) {
    title("5. Manifest compression", "Greedy - Huffman coding");
    const std::string& text = d.manifest;
    auto freq = count_frequencies(text);
    auto root = build_huffman_tree(freq);
    auto codes = build_codes(root.get());
    std::string bits = huffman_encode(text, codes);
    bool lossless = huffman_decode(bits, root.get()) == text;
    double ratio = text.empty() ? 0 : static_cast<double>(bits.size()) / (8.0 * text.size());
    std::cout << "Characters: " << text.size() << " (" << freq.size() << " distinct)\n";
    std::cout << "ASCII: " << 8 * text.size() << " bits  ->  Huffman: " << bits.size() << " bits\n";
    std::cout << std::fixed << std::setprecision(3) << "Compression ratio: " << ratio << "  ("
              << std::setprecision(1) << (1 - ratio) * 100 << "% smaller)\n" << std::defaultfloat;
    std::cout << "Lossless (decoded == original): " << (lossless ? "yes" : "NO") << "\n";
    std::vector<std::pair<char, int>> top(freq.begin(), freq.end());
    std::sort(top.begin(), top.end(), [](auto& a, auto& b) { return a.second > b.second; });
    std::cout << "Most frequent:";
    for (size_t i = 0; i < top.size() && i < 5; i++) {
        std::string c = top[i].first == ' ' ? "space" : top[i].first == '\n' ? "\\n" : std::string(1, top[i].first);
        std::cout << "  " << c << "=" << codes[top[i].first];
    }
    std::cout << "\n";
}

// ---------------- 6. DP: 0/1 knapsack ----------------
static int ratio_greedy_whole(std::vector<Item> items, int W) {       // for comparison only
    std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return static_cast<long long>(a.value) * b.weight > static_cast<long long>(b.value) * a.weight; });
    int v = 0;
    for (const Item& it : items) if (it.weight <= W) { W -= it.weight; v += it.value; }
    return v;
}

static void show_01_knapsack(const CityData& d, int capacity) {
    title("6. Truck loading, whole items only (capacity " + std::to_string(capacity) + " kg)",
          "Dynamic Programming - 0/1 knapsack");
    KnapsackResult r = knapsack_2d(d.cargo, capacity);
    int weight = 0;
    for (int id : r.chosen_ids) {
        std::cout << "  " << std::left << std::setw(22) << d.cargo_names[id] << std::right << std::setw(4)
                  << d.cargo[id].weight << " kg   LKR " << d.cargo[id].value << "\n";
        weight += d.cargo[id].weight;
    }
    std::cout << "Best value: LKR " << r.best_value << "  (" << weight << " kg used)\n";
    std::cout << "Check: 1D rolling array = " << knapsack_1d(d.cargo, capacity)
              << " | ratio greedy (whole items) = " << ratio_greedy_whole(d.cargo, capacity)
              << " | fractional upper bound = " << std::fixed << std::setprecision(0)
              << fractional_knapsack(d.cargo, capacity).total_value << "\n" << std::defaultfloat;
}

// ---------------- 7. DP: Bellman-Ford ----------------
static void print_route(const CityData& d, const ShortestPathResult& r, int src, int dst) {
    std::vector<int> path = get_path(r, src, dst);
    std::cout << "  " << std::left << std::setw(14) << d.area_names[dst] << std::right;
    if (path.empty()) { std::cout << "no route\n"; return; }
    std::cout << std::setw(4) << r.dist[dst] << " min  ";
    for (size_t i = 0; i < path.size(); i++) std::cout << (i ? " -> " : "") << d.area_names[path[i]];
    std::cout << "\n";
}

static void show_routes(const CityData& d, int src, int dst) {      // dst = -1 means "all areas"
    title("7. Cheapest routes from " + d.area_names[src], "Dynamic Programming - Bellman-Ford");
    int V = static_cast<int>(d.area_names.size());
    ShortestPathResult r = bellman_ford(V, d.roads, src);
    if (r.negative_cycle) { std::cout << "Negative cycle found - no cheapest route exists.\n"; return; }
    if (dst >= 0) print_route(d, r, src, dst);
    else for (int v = 0; v < V; v++) if (v != src) print_route(d, r, src, v);
    std::cout << "Rounds used: " << r.rounds << " of " << V - 1 << ", edge checks: " << r.relaxations << "\n";
}

static void choose_route(const CityData& d) {
    int V = static_cast<int>(d.area_names.size());
    std::cout << "\nAreas:";
    for (int i = 0; i < V; i++)
        std::cout << (i % 4 ? "" : "\n  ") << std::setw(2) << i + 1 << ". " << std::left << std::setw(15)
                  << d.area_names[i] << std::right;
    std::cout << "\n";
    int src = read_int("Start area", 1, V, 1) - 1;
    int dst = read_int("Destination (0 = all areas)", 0, V, 0) - 1;
    show_routes(d, src, dst);
}

// ---------------- menu ----------------
static void run_all(const CityData& d) {
    show_sorted_packages(d);
    show_closest_hubs(d);
    show_driver_schedule(d);
    show_fractional(d, 70);
    show_compression(d);
    show_01_knapsack(d, 70);
    show_routes(d, 0, -1);
}

int main(int argc, char* argv[]) {
    std::string data_dir = "data";
    bool demo = false;
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--demo") demo = true;
        else if (a == "--data" && i + 1 < argc) data_dir = argv[++i];
        else { std::cout << "Usage: ./smartcity [--demo] [--data <folder>]\n"; return 1; }
    }

    CityData d;
    try { d = load_city_data(data_dir); }
    catch (const std::exception& e) {
        std::cerr << "Could not load data: " << e.what() << "\n(Run from the project folder, or use --data <folder>.)\n";
        return 1;
    }
    std::cout << "SmartCity Logistics Dashboard\nLoaded " << d.packages.size() << " packages, " << d.hubs.size()
              << " hubs, " << d.jobs.size() << " delivery requests, " << d.cargo.size() << " cargo items, "
              << d.area_names.size() << " areas, " << d.manifest.size() << "-character manifest.\n";

    if (demo) { run_all(d); return 0; }

    while (true) {
        std::cout << "\n------------------------------------------------\n"
                  << " 1. Sort warehouse queue      (Divide & Conquer)\n"
                  << " 2. Find closest hubs         (Divide & Conquer)\n"
                  << " 3. Schedule a driver         (Greedy)\n"
                  << " 4. Load van, split cargo     (Greedy)\n"
                  << " 5. Compress manifest         (Greedy)\n"
                  << " 6. Load truck, whole items   (Dynamic Programming)\n"
                  << " 7. Cheapest delivery route   (Dynamic Programming)\n"
                  << " 8. Run everything (demo)\n"
                  << " 0. Exit\n"
                  << "------------------------------------------------\n";
        int choice = read_int("Choose", 0, 8, 0);
        switch (choice) {
            case 1: show_sorted_packages(d); break;
            case 2: show_closest_hubs(d); break;
            case 3: show_driver_schedule(d); break;
            case 4: show_fractional(d, read_int("Van capacity in kg", 1, 1000, 70)); break;
            case 5: show_compression(d); break;
            case 6: show_01_knapsack(d, read_int("Truck capacity in kg", 1, 1000, 70)); break;
            case 7: choose_route(d); break;
            case 8: run_all(d); break;
            case 0: std::cout << "Goodbye.\n"; return 0;
        }
    }
}