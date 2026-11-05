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

    return 0;
}