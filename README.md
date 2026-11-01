# SmartCity Logistics

A small C++ command-line tool for a delivery company. It loads one day of sample data (packages, hubs, delivery requests, cargo, roads and a text manifest) and answers seven everyday questions using three algorithm design paradigms: Divide and Conquer, Greedy, and Dynamic Programming.

Built for MCT 2101 Advanced Algorithms, Independent Study 01.

## What the program does

When you start the program, it reads the files in `data/` and shows a menu. Each option solves one business problem.

| Option | Business question | Algorithm | Paradigm | Why this paradigm fits |
|---|---|---|---|---|
| 1 | In what order should the warehouse handle packages? (earliest deadline first) | Merge sort | Divide and Conquer | Sort each half on its own, then merge the two sorted halves in one pass |
| 2 | Which two hubs are closest, so they could be merged? | Closest pair of points | Divide and Conquer | Split the map in half, solve each side, then only check a narrow strip near the middle line |
| 3 | Which delivery requests can one driver take, without overlaps, to serve the most customers? | Activity selection | Greedy | Always taking the job that finishes earliest leaves the most time for the rest, and this is proven optimal |
| 4 | How do we fill a van for the most value if cargo can be split? | Fractional knapsack | Greedy | Taking the highest value per kg first is optimal when items can be cut |
| 5 | How can we make the manifest text smaller without losing anything? | Huffman coding | Greedy | Repeatedly joining the two rarest characters gives the shortest prefix-free code |
| 6 | How do we fill a truck for the most value if items cannot be split? | 0/1 knapsack | Dynamic Programming | Greedy can fail here; the same smaller problems (first i items, capacity w) repeat, so we store their answers in a table |
| 7 | What is the cheapest route from the depot to every area? | Bellman-Ford | Dynamic Programming | Best cost using at most k roads is built from the best cost using k-1 roads; it also handles negative costs and detects negative cycles |
| 8 | Run every option once with default values | - | - | Used for the demo and screenshots |

Options 4 and 6 use the same cargo on purpose. At 70 kg, splitting allowed gives LKR 502,400, whole items with DP give LKR 498,500, and a whole-item greedy (best ratio first) gives only LKR 477,500. This shows why greedy is right for the fractional problem but not for the 0/1 problem.

## How to build and run

You need `g++` with C++17 support and `make` (Linux, macOS, or Windows with MSYS2 / WSL).

```bash
make smartcity          # build the dashboard
./smartcity             # interactive menu
./smartcity --demo      # run every option once and exit
./smartcity --data my_folder   # use a different data folder
```

Other commands:

```bash
make tests     # build and run all unit tests
make bench     # build and run the benchmarks, results go to results/*.csv
make all       # dashboard + tests + benchmarks
make clean     # delete the built programs
```

Run the program from the project folder so it can find `data/`.

## Example output

Part of `./smartcity --demo`:

```
SmartCity Logistics Dashboard
Loaded 20 packages, 18 hubs, 18 delivery requests, 15 cargo items, 12 areas, 882-character manifest.

=== 2. Closest pair of hubs (merge candidates)  [Divide & Conquer - closest pair] ===
Closest hubs : H04 Katunayake FTZ  <->  H03 Katunayake Airport
Distance     : 640 m (0.64 km)
Distance checks: D&C 14 vs brute force 153  | same answer: yes

=== 3. One driver's schedule (most deliveries)  [Greedy - earliest finish time] ===
...
Deliveries served: 10 of 18 requests  | optimal (brute-force check): yes
```

Every option prints a check line (for example "same answer: yes" or "Lossless: yes") so you can see that the result is correct.

## Input files

All files are in `data/`. The first line of each CSV file is a header. Times use `HH:MM` (24-hour clock). Weights are whole kilograms.

| File | Columns | Rows | Used by |
|---|---|---|---|
| `sample_packages.csv` | id, destination, deadline, weight_kg, value_lkr | 20 | Option 1 |
| `sample_hubs.csv` | code, name, x_m, y_m (position in metres) | 18 | Option 2 |
| `sample_jobs.csv` | code, customer, start, end | 18 | Option 3 |
| `sample_cargo.csv` | code, description, weight_kg, value_lkr | 15 | Options 4 and 6 |
| `sample_roads.csv` | from, to, minutes, two_way (1 = both directions) | 16 roads, 12 areas | Option 7 |
| `sample_manifest.txt` | plain text | 882 characters | Option 5 |

If a file is missing or a row is wrong, the program stops with a clear message that names the file and line, for example:

```
Could not load data: data/sample_cargo.csv line 3: 'four' is not a whole number
```

Menu input is also checked. Letters or numbers out of range are rejected and the program asks again.

## Complexity summary

| Algorithm | Time | Extra space | Notes |
|---|---|---|---|
| Merge sort | O(n log n) | O(n) | Same in best, average and worst case; uses one temporary array |
| Closest pair, Divide and Conquer | O(n log n) | O(n) | Points sorted once by x and once by y |
| Closest pair, brute force (for checking) | O(n²) | O(1) | Checks every pair |
| Activity selection | O(n log n) | O(n) | Sorting by finish time is the slow part; the selection pass is O(n) |
| Fractional knapsack | O(n log n) | O(n) | Sorting by value per kg is the slow part |
| Huffman coding | O(m + k log k) | O(k) | m = characters in the text, k = distinct characters; uses a min-heap |
| 0/1 knapsack, brute force (for checking) | O(2ⁿ) | O(n) | Tries every take / skip choice |
| 0/1 knapsack, 2D table | O(nW) | O(nW) | Keeps the full table, so it can list which items were chosen |
| 0/1 knapsack, 1D array | O(nW) | O(W) | Same answer with one row, looping capacity from high to low |
| Bellman-Ford | O(VE) | O(V) | Stops early if a round changes nothing |

Letters: n = number of items (packages, hubs, jobs or cargo), W = truck capacity in kg, V = number of areas, E = number of roads.

## Measured results

Produced by `make bench` (files in `results/`). Times depend on the computer; the counts do not.

| Test | What we measured | Result |
|---|---|---|
| Merge sort, n = 1,000 to 100,000 | comparisons / (n log₂ n) | 0.87 to 0.93, so it grows like n log n |
| Closest pair, n = 10,000 | brute force vs Divide and Conquer | 49,995,000 vs 7,725 distance checks, about 51 times faster |
| Huffman, 882-character manifest | bits needed | 4,173 vs 7,056 with 8-bit ASCII (41% smaller), decodes back exactly |
| 0/1 knapsack, n = 25 | brute force calls | 36,526,390 calls, while both DP versions take under 0.01 ms |
| 0/1 knapsack, n = 1,000, W = 10,000 | memory, 2D vs 1D | 40,044,004 vs 40,004 bytes (1,001 times less) |
| Bellman-Ford, V = 5,000, E = 39,998 | edge checks | 399,980 instead of the worst case of about 200 million, thanks to the early stop |

## Tests

`make tests` runs `tests/test_all.cpp`. Each algorithm is checked on small hand-made cases (empty input, one item, ties, edge cases) and on hundreds of random inputs compared with a simple, obviously correct method:

| Module | Compared against |
|---|---|
| Merge sort | sorted order, and ties keep their original order (stable) |
| Closest pair | brute force |
| Activity selection | brute force over all subsets |
| Fractional knapsack | property checks (capacity, at most one split item, never worse than the best 0/1 answer) |
| Huffman | encode then decode gives the original text; codes are prefix-free |
| 0/1 knapsack | brute force = 2D table = 1D array |
| Bellman-Ford | Floyd-Warshall, plus negative-cycle cases |

## Project structure

```
.
├── main.cpp                  menu and output (the dashboard)
├── Makefile
├── common/
│   ├── models.hpp            shared data types: Package, Hub, Job, Item, Edge
│   └── csv_loader.hpp/.cpp   reads data/ files and checks them
├── divide_conquer/           merge_sort, closest_pair
├── greedy/                   activity_selection, fractional_knapsack, huffman
├── dynamic_programming/      knapsack_01, shortest_path_dp (Bellman-Ford)
├── data/                     sample input files
├── tests/test_all.cpp        unit tests
├── bench/benchmarks.cpp      benchmarks that write results/*.csv
├── results/                  benchmark results
└── notes/                    learning log, design notes, greedy proof notes
```

The algorithm files do not read files or print menus. They only take vectors and return results. All file reading is in `csv_loader`, and all printing is in `main.cpp`. This keeps each algorithm easy to test on its own.

## Limitations

- Distances between hubs are straight lines on a flat x/y map, not real road distances.
- Truck capacity must be a whole number of kg (1 to 1000), because the 0/1 knapsack table uses capacity as an index.
- Road costs are minutes in a fixed table; traffic is not modelled.
- A road with a negative cost must be one-way. A two-way negative road would form a negative cycle, so the loader rejects it.

## Author

Akalanka Dilshan 2026/MCT/014 -  MCT 2101 Advanced Algorithms - Independent Study 01 (2026).
