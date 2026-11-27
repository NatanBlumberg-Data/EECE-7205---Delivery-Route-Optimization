#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <random>
#include <cmath>
using namespace std;

vector<vector<double>> loadData() {
    vector<vector<double>> distanceMatrix;
    ifstream file("data/distance_matrix_1.csv");

    if (!file.is_open()) {
        cout << "Error opening CSV file" << endl;
        return distanceMatrix;
    }

    string line;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<double> row;

        getline(ss, value, ',');

        while (getline(ss, value, ',')) {
            if (value.empty() == false) {
                row.push_back(stod(value));
            }
        }

        if (row.size() > 0) {
            distanceMatrix.push_back(row);
        }
    }

    file.close();

    return distanceMatrix;
}


bool isNumber(string value) {
    int digitCount = 0;
    int pointCount = 0;

    for (int i = 0; i < (int)value.size(); i++) {
        if (value[i] >= '0' && value[i] <= '9') {
            digitCount = digitCount + 1;
        }
        else if (value[i] == '.') {
            pointCount = pointCount + 1;
        }
        else if (value[i] != '\r' || i != (int)value.size() - 1) {
            return false;
        }
    }

    if (digitCount == 0 || pointCount > 1) {
        return false;
    }

    return true;
}


bool checkData() {
    ifstream file("data/distance_matrix_1.csv");

    if (!file.is_open()) {
        return true;
    }

    string line;
    vector<string> rowLabels;
    vector<int> valueCounts;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        string rowLabel;
        int valueCount = 0;

        getline(ss, rowLabel, ',');

        while (getline(ss, value, ',')) {
            if (value.empty() == false) {
                if (isNumber(value) == false) {
                    if (value == "\r") {
                        cout << "Error: the file has empty cells and Windows line endings, so save it with LF line endings" << endl;
                    }
                    else {
                        cout << "Error: row " << rowLabel << " has a value that is not a distance: " << value << endl;
                    }

                    file.close();
                    return false;
                }

                valueCount = valueCount + 1;
            }
        }

        if (valueCount > 0) {
            rowLabels.push_back(rowLabel);
            valueCounts.push_back(valueCount);
        }
    }

    file.close();

    int rowCount = (int)rowLabels.size();

    for (int row = 0; row < rowCount; row++) {
        if (valueCounts[row] != rowCount) {
            cout << "Error: row " << rowLabels[row] << " has " << valueCounts[row] << " values but the matrix has " << rowCount << " rows" << endl;
            return false;
        }
    }

    return true;
}


vector<int> nearestNeighbor(const vector<vector<double>>& distanceMatrix) {
    int n = (int)distanceMatrix.size();
    int startLocation = 0;

    vector<int> route;
    vector<bool> visited(n, false);

    route.push_back(startLocation);
    visited[startLocation] = true;

    int currentLocation = startLocation;

    while ((int)route.size() < n) {
        int nearestLocation = -1;
        double shortestDistance = 999999999;

        for (int i = 0; i < n; i++) {
            if (visited[i] == false) {
                double distance = distanceMatrix[currentLocation][i];

                if (distance < shortestDistance) {
                    shortestDistance = distance;
                    nearestLocation = i;
                }
            }
        }

        if (nearestLocation == -1) {
            return route;
        }

        route.push_back(nearestLocation);
        visited[nearestLocation] = true;
        currentLocation = nearestLocation;
    }

    return route;
}


double calculateRouteDistance(const vector<int>& route, const vector<vector<double>>& distanceMatrix) {
    double totalDistance = 0;

    for (int i = 0; i < (int)route.size() - 1; i++) {
        totalDistance += distanceMatrix[route[i]][route[i + 1]];
    }

    totalDistance += distanceMatrix[route[route.size() - 1]][route[0]];

    return totalDistance;
}

vector<int> reverseSegment(const vector<int>& route, int left, int right) {
    vector<int> newRoute = route;

    while (left < right) {
        int temp = newRoute[left];
        newRoute[left] = newRoute[right];
        newRoute[right] = temp;

        left = left + 1;
        right = right - 1;
    }

    return newRoute;
}


double twoOptChange(const vector<int>& route, const vector<vector<double>>& distanceMatrix, int segmentStart, int segmentEnd) {
    int stopCount = route.size();

    int before = route[segmentStart - 1];
    int first = route[segmentStart];
    int last = route[segmentEnd];
    int after;

    if (segmentEnd == stopCount - 1) {
        after = route[0];
    }
    else {
        after = route[segmentEnd + 1];
    }

    double segmentForward = 0;
    double segmentBackward = 0;

    for (int position = segmentStart; position < segmentEnd; position++) {
        segmentForward = segmentForward + distanceMatrix[route[position]][route[position + 1]];
        segmentBackward = segmentBackward + distanceMatrix[route[position + 1]][route[position]];
    }

    double oldEdges = distanceMatrix[before][first] + segmentForward + distanceMatrix[last][after];
    double newEdges = distanceMatrix[before][last] + segmentBackward + distanceMatrix[first][after];

    return newEdges - oldEdges;
}


vector<int> twoOpt(const vector<int>& initialRoute, const vector<vector<double>>& distanceMatrix, int passLimit) {
    vector<int> route = initialRoute;

    int stopCount = route.size();
    int passCount = 0;
    bool routeImproved = true;

    while (routeImproved == true && passCount < passLimit) {
        routeImproved = false;

        int bestStart = -1;
        int bestEnd = -1;
        double bestDifference = 0;

        for (int segmentStart = 1; segmentStart < stopCount - 1; segmentStart++) {
            double segmentForward = 0;
            double segmentBackward = 0;

            for (int segmentEnd = segmentStart + 1; segmentEnd < stopCount; segmentEnd++) {
                segmentForward = segmentForward + distanceMatrix[route[segmentEnd - 1]][route[segmentEnd]];
                segmentBackward = segmentBackward + distanceMatrix[route[segmentEnd]][route[segmentEnd - 1]];

                int before = route[segmentStart - 1];
                int first = route[segmentStart];
                int last = route[segmentEnd];
                int after;

                if (segmentEnd == stopCount - 1) {
                    after = route[0];
                }
                else {
                    after = route[segmentEnd + 1];
                }

                double oldEdges = distanceMatrix[before][first] + segmentForward + distanceMatrix[last][after];
                double newEdges = distanceMatrix[before][last] + segmentBackward + distanceMatrix[first][after];
                double difference = newEdges - oldEdges;

                if (difference < bestDifference) {
                    bestDifference = difference;
                    bestStart = segmentStart;
                    bestEnd = segmentEnd;
                }
            }
        }

        if (bestDifference < -0.000001) {
            route = reverseSegment(route, bestStart, bestEnd);
            routeImproved = true;
        }

        passCount = passCount + 1;
    }

    return route;
}
// -------------------------------------------------------------------------------------//
vector<int> simulatedAnnealing(const vector<int>& initialRoute, const vector<vector<double>>& distanceMatrix, double initialTemperature, double coolingRate, double minimumTemperature, int iterationsPerTemperature) {
    random_device rd;
    mt19937 generator(rd());

    vector<int> currentRoute = initialRoute;
    vector<int> bestRoute = initialRoute;

    double currentDistance = calculateRouteDistance(currentRoute, distanceMatrix);
    double bestDistance = currentDistance;
    double temperature = initialTemperature;

    while (temperature > minimumTemperature) {
        for (int i = 0; i < iterationsPerTemperature; i++) {
            uniform_int_distribution<int> positionDistribution(1, (int)currentRoute.size() - 1);

            int left = positionDistribution(generator);
            int right = positionDistribution(generator);

            while (left == right) {
                right = positionDistribution(generator);
            }

            if (left > right) {
                swap(left, right);
            }

            vector<int> newRoute = reverseSegment(currentRoute, left, right);

            double newDistance = calculateRouteDistance(newRoute, distanceMatrix);
            double difference = newDistance - currentDistance;

            if (difference < 0) {
                currentRoute = newRoute;
                currentDistance = newDistance;
            }

            else {
                double probability = exp(-difference / temperature);
                uniform_real_distribution<double> probabilityDistribution(0.0, 1.0);
                double randomProbability = probabilityDistribution(generator);

                if (randomProbability < probability) {
                    currentRoute = newRoute;
                    currentDistance = newDistance;
                }
            }

            if (currentDistance < bestDistance) {
                bestRoute = currentRoute;
                bestDistance = currentDistance;
            }
        }

        temperature = temperature * coolingRate;
    }
    return bestRoute;
}

//----------------------------------------------------------------------------------------//

int main() {

    if (checkData() == false) {
        return 0;
    }

    vector<vector<double>> distanceMatrix = loadData();

    if (distanceMatrix.empty()) {
        cout << "Error: Distance matrix is empty" << endl;
        return 0;
    }

    vector<int> initialRoute = nearestNeighbor(distanceMatrix);

    if (initialRoute.size() < distanceMatrix.size()) {
        cout << "Error: Nearest-Neighbor could not reach every location" << endl;
        return 0;
    }

    double initialDistance = calculateRouteDistance(initialRoute, distanceMatrix);

    cout << "Initial Route: ";

    for (int i = 0; i < (int)initialRoute.size(); i++) {
        cout << initialRoute[i] << " ";
    }

    cout << initialRoute[0] << endl;

    cout << "Initial Distance: " << initialDistance << endl;

    if (initialRoute.size() < 3) {
        cout << "Fewer than 3 locations, so this is the only route: 2-opt and Simulated Annealing are skipped" << endl;
        return 0;
    }

    // ------------------ 2-opt ---------------------------------------------//

    int passLimit = 10000;

    chrono::steady_clock::time_point timerStart;
    chrono::steady_clock::time_point timerEnd;

    timerStart = chrono::steady_clock::now();
    vector<int> twoOptRoute = twoOpt(initialRoute, distanceMatrix, passLimit);
    timerEnd = chrono::steady_clock::now();
    double twoOptTime = chrono::duration<double, milli>(timerEnd - timerStart).count();

    double twoOptDistance = calculateRouteDistance(twoOptRoute, distanceMatrix);
    double twoOptReduction = initialDistance - twoOptDistance;
    double twoOptPercent = (twoOptReduction / initialDistance) * 100;

    cout << endl;
    cout << "2-opt Route: ";

    for (int position = 0; position < (int)twoOptRoute.size(); position++) {
        cout << twoOptRoute[position] << " ";
    }

    cout << twoOptRoute[0] << endl;

    cout << "2-opt Distance: " << twoOptDistance << endl;
    cout << "2-opt Time: " << twoOptTime << " ms" << endl;
    cout << "2-opt Distance Reduction: " << twoOptReduction << endl;
    cout << "2-opt Percent Reduction: " << twoOptPercent << "%" << endl;

    // ------------------ Simulated Annealing -------------------------------//

    double initialTemperature = 500.0;
    double coolingRate = 0.995;
    double minimumTemperature = 0.01;
    int iterationsPerTemperature = 100;
    int numberOfRuns = 5;

    double totalAnnealingTime = 0;
    double totalAnnealingDistance = 0;
    double bestAnnealingDistance = 999999999;

    vector<int> bestAnnealingRoute;

    for (int run = 0; run < numberOfRuns; run++) {

        timerStart = chrono::steady_clock::now();

        vector<int> annealingRoute = simulatedAnnealing(initialRoute, distanceMatrix, initialTemperature, coolingRate, minimumTemperature, iterationsPerTemperature);

        timerEnd = chrono::steady_clock::now();

        double annealingTime = chrono::duration<double, milli>(timerEnd - timerStart).count();
        double annealingDistance = calculateRouteDistance(annealingRoute, distanceMatrix);

        totalAnnealingTime += annealingTime;
        totalAnnealingDistance += annealingDistance;

        if (annealingDistance < bestAnnealingDistance) {
            bestAnnealingDistance = annealingDistance;
            bestAnnealingRoute = annealingRoute;
        }

        cout << endl;
        cout << "Simulated Annealing Run " << run + 1 << " Distance: " << annealingDistance << endl;
        cout << "Simulated Annealing Run " << run + 1 << " Time: " << annealingTime << " ms" << endl;
    }

    double averageAnnealingTime = totalAnnealingTime / numberOfRuns;
    double averageAnnealingDistance = totalAnnealingDistance / numberOfRuns;
    double annealingReduction = initialDistance - bestAnnealingDistance;
    double annealingPercent = (annealingReduction / initialDistance) * 100;

    cout << endl;
    cout << "Best Simulated Annealing Route: ";

    for (int position = 0; position < (int)bestAnnealingRoute.size(); position++) {
        cout << bestAnnealingRoute[position] << " ";
    }

    cout << bestAnnealingRoute[0] << endl;

    cout << "Average Simulated Annealing Distance: " << averageAnnealingDistance << endl;
    cout << "Best Simulated Annealing Distance: " << bestAnnealingDistance << endl;
    cout << "Average Simulated Annealing Time: " << averageAnnealingTime << " ms" << endl;
    cout << "Best Simulated Annealing Distance Reduction: " << annealingReduction << endl;
    cout << "Best Simulated Annealing Percent Reduction: " << annealingPercent << "%" << endl;

    return 0;
}
