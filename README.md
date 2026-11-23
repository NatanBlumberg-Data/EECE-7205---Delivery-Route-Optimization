# EECE 7205 – Delivery Route Optimizer

A C++ program that plans a delivery route through every location in a pharmaceutical delivery distance matrix and compares route algorithms on total distance, execution time, and distance reduction.

- **Nearest-Neighbor** builds the starting route from location 0 (the depot).
- **2-opt** improves that route by reversing segments while the route gets shorter.
- **Simulated Annealing** improves the same starting route using 2-opt moves.

The distance matrices are one-way: the distance from A to B is usually different from B to A, so 2-opt counts the cost change of every edge inside a reversed segment, not just the two edges it replaces.

## Files

```
src/Source.cpp                 main program
data/distance_matrix_1-9.csv   distance matrices (64 to 85 locations each)
```

## Build and run

Run the program from the repository root, because the program opens `data/...` with a relative path.

```
g++ -std=c++17 -Wall -Wextra -o route_optimizer src/Source.cpp
./route_optimizer
```

The program loads `data/distance_matrix_1.csv` (the file name is set in `loadData`), builds the Nearest-Neighbor route, and improves it with 2-opt and with 5 runs of Simulated Annealing. It prints the Nearest-Neighbor route and distance. For 2-opt and Simulated Annealing, it also prints the time and distance reduction, and for Simulated Annealing it reports the best route and the averages over the 5 runs.

Before loading the file, the program checks it: every value must be a non-negative number, and every row must have one distance per location. If a value or row is wrong, the program prints which one and stops. With fewer than 3 locations there is only one route, so the program prints the Nearest-Neighbor route and skips 2-opt and Simulated Annealing.
