# Greedy Notes

## Fractional vs 0/1 knapsack: one-line note

The ratio greedy is optimal when cargo can be split, because the last item can fill the van exactly. With whole items only, the leftover space is wasted and a different combination can beat it.

## Worked example (van capacity 50 kg)

| item | weight (kg) | value (LKR) | value/weight |
|------|-------------|-------------|--------------|
| 1    | 10          | 60          | 6            |
| 2    | 20          | 100         | 5            |
| 3    | 30          | 120         | 4            |

| method                                   | what it loads                           | total value (LKR) |
|------------------------------------------|-----------------------------------------|-------------------|
| Fractional greedy (items can be split)   | items 1 and 2 whole, 2/3 of item 3      | **240**           |
| Ratio greedy, whole items only           | items 1 and 2; item 3 (30 kg) won't fit in the last 20 kg | **160** |
| True 0/1 optimum (brute force)           | items 2 and 3                           | **220**           |

Fractional: 60 + 100 + (20/30) × 120 = 240.

Greedy on whole items stops at 160, below the optimum of 220. Taking the best-ratio item first ends up blocking the best combination, so 0/1 knapsack needs dynamic programming (Week 2).

Reproduce with `make bench`; the numbers come from `demo_knapsack_greedy()` in `bench/benchmarks.cpp`.
