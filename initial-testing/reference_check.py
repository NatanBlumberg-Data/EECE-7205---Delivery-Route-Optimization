import csv, itertools, sys

def load(fileName):
    rows = list(csv.reader(open(fileName)))[1:]
    matrix = []
    for row in rows:
        values = [float(cell) for cell in row[1:] if cell.strip() != ""]
        if values:
            matrix.append(values)
    return matrix

def routeDistance(route, matrix):
    total = 0
    for i in range(len(route)):
        total += matrix[route[i]][route[(i + 1) % len(route)]]
    return total

def nearestNeighbor(matrix):
    n = len(matrix)
    route = [0]
    visited = {0}
    current = 0
    while len(route) < n:
        best = None
        for i in range(n):
            if i not in visited and (best is None or matrix[current][i] < matrix[current][best]):
                best = i
        route.append(best)
        visited.add(best)
        current = best
    return route

def reverse(route, start, end):
    return route[:start] + route[start:end + 1][::-1] + route[end + 1:]

def twoOpt(route, matrix):
    n = len(route)
    while True:
        base = routeDistance(route, matrix)
        bestChange, bestStart, bestEnd = 0, None, None
        for start in range(1, n - 1):
            for end in range(start + 1, n):
                change = routeDistance(reverse(route, start, end), matrix) - base
                if change < bestChange:
                    bestChange, bestStart, bestEnd = change, start, end
        if bestChange < -0.000001:
            route = reverse(route, bestStart, bestEnd)
        else:
            return route

def bestRemainingMove(route, matrix):
    base = routeDistance(route, matrix)
    best = 0
    for start in range(1, len(route) - 1):
        for end in range(start + 1, len(route)):
            best = min(best, routeDistance(reverse(route, start, end), matrix) - base)
    return best

matrix = load(sys.argv[1])
n = len(matrix)
nnRoute = nearestNeighbor(matrix)
twoOptRoute = twoOpt(nnRoute, matrix)
print("Locations:", n)
print("Nearest-Neighbor route:", " ".join(map(str, nnRoute + [0])))
print("Nearest-Neighbor distance:", round(routeDistance(nnRoute, matrix), 4))
print("2-opt route:", " ".join(map(str, twoOptRoute + [0])))
print("2-opt distance:", round(routeDistance(twoOptRoute, matrix), 4))
print("Best remaining reversal change (0 = local optimum):", round(bestRemainingMove(twoOptRoute, matrix), 6))
if n <= 9:
    best = None
    for order in itertools.permutations(range(1, n)):
        route = [0] + list(order)
        if best is None or routeDistance(route, matrix) < routeDistance(best, matrix):
            best = route
    print("Brute-force optimum:", " ".join(map(str, best + [0])), round(routeDistance(best, matrix), 4))
