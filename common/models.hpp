#pragma once
#include <string>

// A package waiting in the warehouse queue
struct Package {
    int id;
    std::string destination;
    int deadline;      // hours from now; if it is smaller mean its more urgent
    double weight;     // kg
    double value;      // LKR
};

// A delivery hub on a 2D city map (used by closest pair)
struct Hub {
    int id;
    double x;   // metres east on the city map
    double y;   // metres north on the city map
};

// A delivery time window for one driver (used by activity selection)
struct Job {
    int id;
    int start;   // e.g. minutes from shift start, or 24h clock hour
    int end;     // must be >= start
};