#include "merge_sort.hpp"

// Merge two sorted halves
static void merge(std::vector<Package>& a, std::vector<Package>& temp,int low, int mid, int high, long long& comparisons) {
    int i = low;       // index in left half
    int j = mid + 1;   // index in right half
    int k = low;       // index in temp

    while (i <= mid && j <= high) {
        comparisons++;
        // '<=' keeps equal deadlines in original order (stable sort)
        if (a[i].deadline <= a[j].deadline) {
            temp[k++] = a[i++];
        } else {
            temp[k++] = a[j++];
        }
    }
    while (i <= mid)  temp[k++] = a[i++];   // copy other already sorted part to temp
    while (j <= high) temp[k++] = a[j++];

    for (int x = low; x <= high; x++) a[x] = temp[x];
}

// Divide
static void merge_sort_rec(std::vector<Package>& a, std::vector<Package>& temp,int low, int high, long long& comparisons) {
    if (low >= high) return;               // base case: 0 or 1 element, Conquer
    int mid = low + (high - low) / 2;
    merge_sort_rec(a, temp, low, mid, comparisons);
    merge_sort_rec(a, temp, mid + 1, high, comparisons);
    merge(a, temp, low, mid, high, comparisons); //merge part
}

void merge_sort(std::vector<Package>& packages, long long& comparisons) {
    if (packages.size() < 2) return;
    
    std::vector<Package> temp(packages.size());   // create temp vector with original Package vector size
    
    merge_sort_rec(packages, temp, 0, static_cast<int>(packages.size()) - 1, comparisons);
}