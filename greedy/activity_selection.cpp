#include "activity_selection.hpp"
#include <algorithm>
#include <climits>

std::vector<Job> activity_selection(const std::vector<Job>& jobs) {
    std::vector<Job> sorted = jobs;                       // copy: keep caller's data
    // Step 1: sort by finish time (ties: earlier start first, for a stable order)
    std::sort(sorted.begin(), sorted.end(), [](const Job& a, const Job& b) { //Sorting Engine Comparator-lambda function
        if (a.end != b.end) return a.end < b.end;  //Rule 1 , ekma time eke iwara wenwanm withrai rule 2 ekt ynne
        return a.start < b.start; // rule 2
    });

    std::vector<Job> chosen;
    int last_end = INT_MIN;                               
   
    for (const Job& j : sorted) {
        if (j.start >= last_end) {                        // greedy choice
            chosen.push_back(j);
            last_end = j.end;
        }
    }
    return chosen;
}

int max_jobs_brute_force(const std::vector<Job>& jobs) {
    int n = static_cast<int>(jobs.size());
    int best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {         // every subset
        std::vector<Job> pick;
        for (int i = 0; i < n; i++)
            if (mask & (1 << i)) pick.push_back(jobs[i]);
        std::sort(pick.begin(), pick.end(),
                  [](const Job& a, const Job& b) { return a.start < b.start; });
        bool ok = true;
        for (size_t i = 1; i < pick.size() && ok; i++)
            if (pick[i].start < pick[i - 1].end) ok = false;   // overlap
        if (ok) best = std::max(best, static_cast<int>(pick.size()));
    }
    return best;
}