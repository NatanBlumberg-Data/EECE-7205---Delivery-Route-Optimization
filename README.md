# EECE 7205 – Delivery Route Optimizer

A C++ program that plans a delivery route through every location in a pharmaceutical delivery distance matrix and compares route algorithms on total distance, execution time, and distance reduction.

- **Nearest-Neighbor** builds the starting route from location 0 (the depot).
- **2-opt** improves that route by reversing segments while the route gets shorter.
- **Simulated Annealing** (in progress) will improve the same starting route using 2-opt moves.

The distance matrices are one-way: the distance from A to B is usually different from B to A, so 2-opt counts the cost change of every edge inside a reversed segment, not just the two edges it replaces.

## Files

```
Source.cpp                     main program
data/distance_matrix_1-9.csv   distance matrices (64 to 85 locations each)
```

## Build and run

Run the program from the repository root, because the program opens `data/...` with a relative path.

```
g++ -std=c++17 -Wall -Wextra -o route_optimizer Source.cpp
./route_optimizer
```

The program loads `data/distance_matrix_1.csv` (the file name is set in `loadData`), builds the Nearest-Neighbor route, improves it with 2-opt, and prints both routes and distances along with the 2-opt time and distance reduction.
