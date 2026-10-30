# Learning Log

A running record of what I did, what broke, how I fixed it, and what I learned.
Newest entries at the top. Keep each cell short; link out for details.

| time | task | problem | fix | insight |
|------|------|---------|-----|---------|
| 2026-10-30 10:45 | Benchmarked merge sort (n = 1k to 100k) → `results/merge_sort.csv` | Wall-clock time alone is noisy and doesn't prove the complexity class | Counted comparisons inside merge and compared them to n·log2(n); fixed the RNG seed (42) so runs are repeatable | Ratio stays between 0.87 and 0.93 and creeps toward 1 as n grows, which fits Θ(n log n). 10× more input gave about 10× more time (1.7 ms → 17.7 ms), so there is no quadratic blow-up. |
| 2026-10-30 10:33 | Wrote merge sort test cases (`tests/test_all.cpp`) | Hard to know which edge cases matter for a sort | Covered 5 cases: empty, one element, already sorted, reverse sorted, duplicate deadlines | The duplicate-deadline test is the one that proves stability (ties keep their original order). Sorted/reverse inputs catch bad merge indexing. |
| 2026-10-30 10:07 | Created `Package` model in `common/models.hpp` | Git showed a rename of `models.hpp ` → `models.hpp`: the filename had a trailing space | Renamed the file and committed it as a separate fix | Check filenames for stray whitespace. Keep a fix like this in its own commit so history stays clear. |
| 2026-10-30 10:30 | Finished merge sort function | - | - | **Divide:** split the list at mid.<br>**Conquer:** sort each half recursively.<br>**Combine:** merge two sorted halves in linear time.<br>**Recurrence:** T(n) = 2T(n/2) + Θ(n) → Θ(n log n) (Master Theorem, case 2).<br>Count one comparison each time two deadlines are compared inside merge.<br>Use `<=` so equal deadlines keep their original order (stable sort). |
| 2026-10-30 07:40 | Finished Makefile | - | - | Learned how to handle an intermediate-level project Makefile |
| 2026-10-29 19:00 | Set up project folder structure | Lack of knowledge on how to organize an intermediate-level C++ project | Looked at some open-source GitHub projects and researched with AI agents | If I struggle with organizing a project, look up someone's git repo, ask an AI agent for the industry standard, then combine both and reorganize my own way. |
