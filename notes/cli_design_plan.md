```text
data/*.csv, sample_manifest.txt
        |
common/csv_loader.cpp   -> load_city_data() -> CityData (vectors + name lists)
        |
main.cpp  menu -> show_xxx() -> merge_sort / closest_pair / activity_selection /
                                fractional_knapsack / huffman / knapsack_2d + 1d / bellman_ford
```
algorithms work on ids (0, 1, 2 ...). The loader gives each row id = its row position and keeps the display names in a parallel vector (cargo_names[id]). That way the algorithm files stay unchanged and the dashboard can still print "C02 Laptop box"

1. Create the sample data files
2. Create csv_loader
3. Main part is rewrite main c++ file