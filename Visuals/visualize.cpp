#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>
using namespace std;
using Matrix=vector<vector<double>>;
using Route=vector<int>;
struct Point{double x,y;};
Matrix readCSV(const string& path){
 ifstream f(path); if(!f) throw runtime_error("Cannot open dataset");
 string line,cell; getline(f,line); Matrix d;
 while(getline(f,line)){if(line.empty())continue; stringstream s(line); getline(s,cell,','); if(stoi(cell)!=(int)d.size())throw runtime_error("Unexpected row ID"); vector<double> row; while(getline(s,cell,','))row.push_back(stod(cell)); d.push_back(row);}
 if(d.size()<3)throw runtime_error("Too few locations"); for(auto& r:d){if(r.size()!=d.size())throw runtime_error("Matrix not square"); for(double v:r)if(!isfinite(v)||v<0)throw runtime_error("Invalid distance");} return d;
}
// Original team algorithm functions from Source.cpp, unchanged.
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

// Classical MDS: double-center squared symmetric distances, then extract
// the two largest positive eigenpairs using shifted power iteration.
vector<Point> layout(const Matrix& d){int n=d.size();Matrix b(n,vector<double>(n));vector<double> mean(n);double grand=0;for(int i=0;i<n;++i)for(int j=0;j<n;++j){double v=(d[i][j]+d[j][i])/2;b[i][j]=v*v;mean[i]+=b[i][j]/n;grand+=b[i][j]/(n*n);}for(int i=0;i<n;++i)for(int j=0;j<n;++j)b[i][j]=-.5*(b[i][j]-mean[i]-mean[j]+grand);double shift=0;for(auto& row:b){double s=0;for(double v:row)s+=abs(v);shift=max(shift,s);}vector<vector<double>> axes;vector<Point> p(n);for(int a=0;a<2;++a){vector<double> v(n),w(n);for(int i=0;i<n;++i)v[i]=sin((i+1)*(a+1)*1.37);for(int k=0;k<2000;++k){for(int i=0;i<n;++i)w[i]=inner_product(b[i].begin(),b[i].end(),v.begin(),0.0)+shift*v[i];for(auto& q:axes){double dot=inner_product(w.begin(),w.end(),q.begin(),0.0);for(int i=0;i<n;++i)w[i]-=dot*q[i];}double norm=sqrt(inner_product(w.begin(),w.end(),w.begin(),0.0));if(norm<1e-12)throw runtime_error("Degenerate layout");for(int i=0;i<n;++i)v[i]=w[i]/norm;}double eig=0;for(int i=0;i<n;++i)eig+=v[i]*inner_product(b[i].begin(),b[i].end(),v.begin(),0.0);axes.push_back(v);for(int i=0;i<n;++i)(a?p[i].y:p[i].x)=v[i]*sqrt(max(0.0,eig));}return p;}
int main(int argc,char** argv){try{Matrix d=readCSV(argc>1?argv[1]:"data/distance_matrix_3.csv");auto nn=nearestNeighbor(d); auto opt=twoOpt(nn,d,10000); Route sa; double best=1e100; for(int run=0;run<5;++run){auto r=simulatedAnnealing(nn,d,500.0,0.995,0.01,100); double c=calculateRouteDistance(r,d); cout<<"SA run "<<run+1<<": "<<c<<"\n"; if(c<best){best=c;sa=r;}}auto p=layout(d);double xmin=p[0].x,xmax=xmin,ymin=p[0].y,ymax=ymin;for(auto q:p){xmin=min(xmin,q.x);xmax=max(xmax,q.x);ymin=min(ymin,q.y);ymax=max(ymax,q.y);}double scale= min(410/max(1e-9,xmax-xmin),330/max(1e-9,ymax-ymin));for(auto& q:p){q.x=240+(q.x-(xmin+xmax)/2)*scale;q.y=270-(q.y-(ymin+ymax)/2)*scale;}
 ofstream f("Day3RouteComparison.svg");if(!f)throw runtime_error("Cannot write SVG");f<<fixed<<setprecision(2)<<"<svg xmlns='http://www.w3.org/2000/svg' width='1440' height='535' viewBox='0 0 1440 535'><rect width='1440' height='535' fill='white'/><g font-family='Arial,sans-serif' fill='#172334'><text x='720' y='32' text-anchor='middle' font-size='24' font-weight='bold'>Day 3 | Delivery route optimization</text><text x='720' y='55' text-anchor='middle' font-size='14'>"<<d.size()<<" locations including depot | Same estimated positions in every panel</text>";
 vector<Route> routes{nn,opt,sa};vector<string> names{"Nearest-neighbor","2-opt","Simulated annealing (best of 5)"},colors{"#64748b","#2563eb","#16805c"};for(int a=0;a<3;++a){double c=calculateRouteDistance(routes[a],d);cout<<names[a]<<": "<<c<<"\n";f<<"<g transform='translate("<<480*a<<",0)'><text x='240' y='87' text-anchor='middle' font-size='20' font-weight='bold'>"<<names[a]<<"</text><polyline fill='none' stroke='"<<colors[a]<<"' stroke-width='1.7' stroke-opacity='.8' points='";for(int i:routes[a])f<<p[i].x<<","<<p[i].y<<" ";f<<p[0].x<<","<<p[0].y<<"'/>";for(size_t i=0;i<p.size();++i)f<<"<circle cx='"<<p[i].x<<"' cy='"<<p[i].y<<"' r='"<<(i==0?6:3)<<"' fill='"<<(i==0?"#e4572e":"#172334")<<"'><title>Location "<<i<<"</title></circle>";f<<"<text x='240' y='465' text-anchor='middle' font-size='19' font-weight='bold'>Distance: "<<c<<"</text><text x='240' y='487' text-anchor='middle' font-size='14'>Reduction: "<<100*(1-c/calculateRouteDistance(nn,d))<<"%</text></g>";}f<<"<text x='720' y='520' text-anchor='middle' font-size='13'>Orange = depot | Positions estimated by classical MDS; lines are route order, not roads. Costs use original directed matrix.</text></g></svg>";
 ofstream csv("Day3Routes.csv");csv<<"algorithm,step,location\n";for(int a=0;a<3;++a)for(size_t i=0;i<=routes[a].size();++i)csv<<names[a]<<','<<i<<','<<routes[a][i%routes[a].size()]<<'\n';
 }catch(const exception& e){cerr<<e.what()<<'\n';return 1;}}
