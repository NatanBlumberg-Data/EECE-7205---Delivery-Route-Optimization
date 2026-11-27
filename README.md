# EECE 7205 – Delivery Route Optimizer

A C++ program that plans a delivery route through every location in a pharmaceutical delivery distance matrix and compares route algorithms on total distance, execution time, and distance reduction.

- **Nearest-Neighbor** builds the starting route from location 0 (the depot).
- **2-opt** improves that route by reversing segments while the route gets shorter.
- **Simulated Annealing** improves the same starting route using 2-opt moves.

The distance matrices are one-way: the distance from A to B is usually different from B to A, so 2-opt counts the cost change of every edge inside a reversed segment, not just the two edges it replaces.

## Files

```
src/Source.cpp                         main program
data/distance_matrix_1-9.csv           distance matrices (64 to 85 locations each)
initial-testing/                       inputs for Tests 1-12
    test1_normal/data/                 normal input: 5 locations
    test2_two_locations/data/          boundary case: 2 locations
    test3_one_location/data/           boundary case: depot only
    test4_non_numeric/data/            invalid input: N/A in one cell
    test5_missing_cell/data/           invalid input: one blank cell
    test6_empty/data/                  empty input: 0-byte file
    test7_unreachable/data/            no-solution case: inf in one column
    test8_largest/data/                larger input: copy of distance_matrix_9.csv (85 locations)
    test9_three_locations/data/        boundary case: 3 locations
    test10_zero_distances/data/        boundary case: every distance is 0
    test11_negative/data/              invalid input: a negative distance
    test12_far_location/data/          no-solution case: location 3 is 1,000,000,000 from every location
    reference_check.py                 independent Python check of Nearest-Neighbor and 2-opt
```

Each test folder holds its matrix as `data/distance_matrix_1.csv`, the file name the program opens.

## Data source

The distance matrices come from *Delivering Data: A Real-World Dataset for Last-Mile Delivery Optimization* by Anna Vrani, Savvas Apostolidis, Athanasios Kapoutsis and Elias Kosmatopoulos (Centre for Research and Technology Hellas), published on Zenodo in 2025 under the CC BY 4.0 license. The dataset covers about one month of pharmaceutical deliveries, split into nine daily instances. We use only its nine distance matrices (distances in kilometers), saved as CSV files.

- Zenodo: https://zenodo.org/records/15310106 (DOI 10.5281/zenodo.15310106)
- GitHub: https://github.com/annavrani/Delivering-Data-A-Real-World-Dataset-for-Last-Mile-Delivery-Optimization

## Requirements

- A C++ compiler with C++17 support. We tested with g++ 13.3.
- Python 3, only for `reference_check.py`. It uses no extra packages.
- A terminal for the commands below: Linux, macOS, or WSL on Windows.

## Build and run

Run the program from the repository root, because the program opens `data/...` with a relative path.

```
g++ -std=c++17 -Wall -Wextra -o route_optimizer src/Source.cpp
./route_optimizer
```

The program runs one day at a time. It loads `data/distance_matrix_1.csv` (Day 1), builds the Nearest-Neighbor route, and improves it with 2-opt and with 5 runs of Simulated Annealing. It prints the Nearest-Neighbor route and distance. For 2-opt and Simulated Annealing, it also prints the time and distance reduction, and for Simulated Annealing it reports the best route and the averages over the 5 runs.

Before loading the file, the program checks it: every value must be a non-negative number, and every row must have one distance per location. If a value or row is wrong, the program prints which one and stops. With fewer than 3 locations there is only one route, so the program prints the Nearest-Neighbor route and skips 2-opt and Simulated Annealing.

To run a different day, change `data/distance_matrix_1.csv` to that day's file, for example `data/distance_matrix_5.csv`, in both `checkData` and `loadData`, then build again. If only `loadData` is changed, the program runs the new day without checking its file first.

## Testing

Build the program from the repository root, then run a test from inside its folder, because each test folder has its own `data/distance_matrix_1.csv`:

```
cd initial-testing/test1_normal
../../route_optimizer
```

To run all twelve tests in order from the repository root:

```
for test in 1 2 3 4 5 6 7 8 9 10 11 12; do
    folder=$(ls -d initial-testing/test${test}_*)
    echo "===== $folder"
    (cd "$folder" && ../../route_optimizer)
done
```

To check a matrix with the Python reference:

```
python3 initial-testing/reference_check.py initial-testing/test8_largest/data/distance_matrix_1.csv
```

It prints the Nearest-Neighbor and 2-opt routes and distances, and the best change any remaining reversal could make (0 means the 2-opt route cannot be shortened by a reversal). For 9 or fewer locations, it also prints the shortest possible route, found by trying every route.

### Expected results

NN is Nearest-Neighbor and SA is Simulated Annealing. SA uses a random seed, so its routes, distances and times can change between runs. Times also depend on the computer. Every other result below is the same on every run.

| Test | Input | Expected result |
|---|---|---|
| 1 | 5 locations, one-way distances | NN 0 4 1 2 3 0, distance 33. 2-opt 0 4 3 1 2 0, distance 26, the shortest possible route. SA best between 26 and 33. |
| 2 | 2 locations | Route 0 1 0, distance 10.5, and a note that 2-opt and SA are skipped. |
| 3 | Depot only | Route 0 0, distance 0, and the same note. |
| 4 | `N/A` in one cell | `Error: row 0 has a value that is not a distance: N/A` |
| 5 | One blank cell | `Error: row 1 has 3 values but the matrix has 4 rows` |
| 6 | 0-byte file | `Error: Distance matrix is empty` |
| 7 | `inf` in one column | `Error: row 0 has a value that is not a distance: inf` |
| 8 | Dataset 9, 85 locations | NN distance 194.8 and 2-opt distance 183.3, with the same routes as `reference_check.py`. SA best at most 194.8. |
| 9 | 3 locations | NN 0 1 2 0, distance 12. 2-opt 0 2 1 0, distance 6. SA best 6. |
| 10 | Every distance is 0 | Every route has distance 0. Reductions are 0, and both percent reductions are 0%. |
| 11 | A negative distance | `Error: row 1 has a value that is not a distance: -3` |
| 12 | Location 3 is 1,000,000,000 from every location | `Error: Nearest-Neighbor could not reach every location` |

## Acknowledgments

*Placeholder: external resources and AI tools used (to be added).*
