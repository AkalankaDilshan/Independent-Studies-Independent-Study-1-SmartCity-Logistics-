#pragma once
#include <string>
#include <vector>
#include "models.hpp"

struct CityData {
    std::vector<Package> packages;
    std::vector<Hub> hubs;           std::vector<std::string> hub_names;
    std::vector<Job> jobs;           std::vector<std::string> job_names;
    std::vector<Item> cargo;         std::vector<std::string> cargo_names;
    std::vector<Edge> roads;         std::vector<std::string> area_names;
    std::string manifest;
};

// "HH:MM" -> minutes after midnight (e.g. "09:30" -> 570). Throws on bad input.
int parse_time(const std::string& hhmm);
// minutes after midnight -> "HH:MM"
std::string format_time(int minutes);

// Loads all sample files from a folder (default "data").
// Throws std::runtime_error with the file name and line number if something is wrong.
CityData load_city_data(const std::string& dir);