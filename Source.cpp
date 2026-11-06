#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
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


vector<int> nearestNeighbor(vector<vector<double>> distanceMatrix) {
    int n = distanceMatrix.size();
    int startLocation = 0;

    vector<int> route;
    vector<bool> visited(n, false);

    route.push_back(startLocation);
    visited[startLocation] = true;

    int currentLocation = startLocation;

    while (route.size() < n) {
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

        route.push_back(nearestLocation);
        visited[nearestLocation] = true;
        currentLocation = nearestLocation;
    }

    return route;
}


double calculateRouteDistance(vector<int> route, vector<vector<double>> distanceMatrix) {
    double totalDistance = 0;

    for (int i = 0; i < route.size() - 1; i++) {
        totalDistance += distanceMatrix[route[i]][route[i + 1]];
    }

    totalDistance += distanceMatrix[route[route.size() - 1]][route[0]];

    return totalDistance;
}

vector<int> reverseSegment(vector<int> route, int left, int right) {
    while (left < right) {
        int temp = route[left];
        route[left] = route[right];
        route[right] = temp;

        left = left + 1;
        right = right - 1;
    }

    return route;
}


double twoOptChange(vector<int> route, vector<vector<double>> distanceMatrix, int segmentStart, int segmentEnd) {
    int stopCount = route.size();

    int before = route[segmentStart - 1];
    int first = route[segmentStart];
    int last = route[segmentEnd];
    int after;

    if (segmentEnd == stopCount - 1) {
        after = route[0];
    } else {
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


vector<int> twoOpt(vector<int> route, vector<vector<double>> distanceMatrix, int passLimit) {
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
                } else {
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

int main() {

    vector<vector<double>> distanceMatrix = loadData();

    if (distanceMatrix.empty()) {
        cout << "Error: Distance matrix is empty" << endl;
        return 0;
    }

    vector<int> initialRoute = nearestNeighbor(distanceMatrix);

    double initialDistance = calculateRouteDistance(initialRoute, distanceMatrix);

    cout << "Initial Route: ";

    for (int i = 0; i < initialRoute.size(); i++) {
        cout << initialRoute[i] << " ";
    }

    cout << initialRoute[0] << endl;

    cout << "Initial Distance: " << initialDistance << endl;

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

    return 0;
}
