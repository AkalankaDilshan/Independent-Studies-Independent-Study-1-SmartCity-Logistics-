#include "csv_loader.hpp"
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

// ---------- small helpers ----------
static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Reads a CSV file into rows of fields. Skips the header line and empty lines.
// Each row also remembers its line number for error messages.
struct CsvRow { int line; std::vector<std::string> f; };

static std::vector<CsvRow> read_csv(const std::string& path, size_t expected_cols) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    std::vector<CsvRow> rows;
    std::string line;
    int line_no = 0;
    std::getline(in, line);                                   // header
    line_no++;
    while (std::getline(in, line)) {
        line_no++;
        if (trim(line).empty()) continue;
        CsvRow row{line_no, {}};
        std::stringstream ss(line);
        std::string field;
        while (std::getline(ss, field, ',')) row.f.push_back(trim(field));
        if (row.f.size() != expected_cols)
            throw std::runtime_error(path + " line " + std::to_string(line_no) + ": expected " +
                                     std::to_string(expected_cols) + " columns, found " +
                                     std::to_string(row.f.size()));
        rows.push_back(row);
    }
    return rows;
}

// Number conversion with a clear error message instead of a crash
static int to_int(const CsvRow& r, size_t i, const std::string& path) {
    try { size_t pos; int v = std::stoi(r.f[i], &pos); if (pos == r.f[i].size()) return v; } catch (...) {}
    throw std::runtime_error(path + " line " + std::to_string(r.line) + ": '" + r.f[i] + "' is not a whole number");
}
static double to_double(const CsvRow& r, size_t i, const std::string& path) {
    try { size_t pos; double v = std::stod(r.f[i], &pos); if (pos == r.f[i].size()) return v; } catch (...) {}
    throw std::runtime_error(path + " line " + std::to_string(r.line) + ": '" + r.f[i] + "' is not a number");
}

int parse_time(const std::string& hhmm) {
    int h = -1, m = -1;
    char colon = 0;
    std::stringstream ss(hhmm);
    if (!(ss >> h >> colon >> m) || colon != ':' || h < 0 || h > 23 || m < 0 || m > 59)
        throw std::runtime_error("bad time '" + hhmm + "' (use HH:MM)");
    return h * 60 + m;
}

std::string format_time(int minutes) {
    char buf[16];   // room for any int, so -Wformat-truncation stays quiet
    std::snprintf(buf, sizeof(buf), "%02d:%02d", (minutes / 60) % 24, minutes % 60);
    return buf;
}

static std::string with_line(const std::string& path, int line, const std::string& msg) {
    return path + " line " + std::to_string(line) + ": " + msg;
}

// ---------- one loader per file ----------
CityData load_city_data(const std::string& dir) {
    CityData d;

    // packages: id,destination,deadline,weight_kg,value_lkr
    std::string p = dir + "/sample_packages.csv";
    for (const CsvRow& r : read_csv(p, 5)) {
        int deadline;
        try { deadline = parse_time(r.f[2]); }
        catch (const std::exception& e) { throw std::runtime_error(with_line(p, r.line, e.what())); }
        d.packages.push_back({to_int(r, 0, p), r.f[1], deadline, to_double(r, 3, p), to_double(r, 4, p)});
    }

    // hubs: code,name,x_m,y_m  (id = row position, so results map back to names)
    p = dir + "/sample_hubs.csv";
    for (const CsvRow& r : read_csv(p, 4)) {
        d.hubs.push_back({static_cast<int>(d.hubs.size()), to_double(r, 2, p), to_double(r, 3, p)});
        d.hub_names.push_back(r.f[0] + " " + r.f[1]);
    }

    // jobs: code,customer,start,end
    p = dir + "/sample_jobs.csv";
    for (const CsvRow& r : read_csv(p, 4)) {
        int s, e;
        try { s = parse_time(r.f[2]); e = parse_time(r.f[3]); }
        catch (const std::exception& ex) { throw std::runtime_error(with_line(p, r.line, ex.what())); }
        if (e < s) throw std::runtime_error(with_line(p, r.line, "end time is before start time"));
        d.jobs.push_back({static_cast<int>(d.jobs.size()), s, e});
        d.job_names.push_back(r.f[0] + " " + r.f[1]);
    }

    // cargo: code,description,weight_kg,value_lkr
    p = dir + "/sample_cargo.csv";
    for (const CsvRow& r : read_csv(p, 4)) {
        int w = to_int(r, 2, p), v = to_int(r, 3, p);
        if (w < 0 || v < 0) throw std::runtime_error(with_line(p, r.line, "weight and value must be >= 0"));
        d.cargo.push_back({static_cast<int>(d.cargo.size()), w, v});
        d.cargo_names.push_back(r.f[0] + " " + r.f[1]);
    }

    // roads: from,to,minutes,two_way  (area names become node numbers)
    p = dir + "/sample_roads.csv";
    std::map<std::string, int> area_id;
    auto id_of = [&](const std::string& name) {
        auto it = area_id.find(name);
        if (it != area_id.end()) return it->second;
        int id = static_cast<int>(d.area_names.size());
        area_id[name] = id;
        d.area_names.push_back(name);
        return id;
    };
    for (const CsvRow& r : read_csv(p, 4)) {
        int a = id_of(r.f[0]), b = id_of(r.f[1]);
        int cost = to_int(r, 2, p), two_way = to_int(r, 3, p);
        if (two_way && cost < 0)
            throw std::runtime_error(with_line(p, r.line, "a two-way road cannot have a negative cost (negative cycle)"));
        d.roads.push_back({a, b, cost});
        if (two_way) d.roads.push_back({b, a, cost});
    }

    // manifest: plain text
    p = dir + "/sample_manifest.txt";
    std::ifstream in(p);
    if (!in) throw std::runtime_error("cannot open " + p);
    std::stringstream ss;
    ss << in.rdbuf();
    d.manifest = ss.str();
    return d;
}