// Reproduce Iteration 3 sample results; not a live benchmark.
// Build: g++ -std=c++17 visualize.cpp -o visualize
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>
#include <filesystem>
#include <stdexcept>
using namespace std;

namespace fs = std::filesystem;

string number(double x, int precision=2) {
    ostringstream s; s << fixed << setprecision(precision) << x; return s.str();
}
struct SVG {
    ofstream out;
    SVG(const fs::path& file, string title) : out(file) {
        if (!out) throw runtime_error("Cannot open " + file.string());
        out << "<svg xmlns='http://www.w3.org/2000/svg' width='960' height='520' viewBox='0 0 960 520'>\n";
        out << "<rect width='960' height='520' fill='white'/>\n";
        text(480,38,title,25);
    }
    ~SVG(){out << "</svg>\n";}
    void text(double x,double y,string s,int size=16,string color="#243247",string anchor="middle") {
        out << "<text x='"<<x<<"' y='"<<y<<"' font-family='Arial,sans-serif' font-size='"<<size<<"' fill='"<<color<<"' text-anchor='"<<anchor<<"'>"<<s<<"</text>\n";
    }
    void line(double x,double y,double a,double b,string c="#d5dbe3",int w=1,bool dash=false) {
        out<<"<line x1='"<<x<<"' y1='"<<y<<"' x2='"<<a<<"' y2='"<<b<<"' stroke='"<<c<<"' stroke-width='"<<w<<"'"<<(dash?" stroke-dasharray='7 5'":"")<<"/>\n";
    }
    void rect(double x,double y,double w,double h,string c){out<<"<rect x='"<<x<<"' y='"<<y<<"' width='"<<w<<"' height='"<<h<<"' rx='3' fill='"<<c<<"'/>\n";}
    void circle(double x,double y,double r,string c){out<<"<circle cx='"<<x<<"' cy='"<<y<<"' r='"<<r<<"' fill='"<<c<<"'/>\n";}
};
const string blue="#2563eb", green="#15803d", gray="#64748b";
void bars(const fs::path& file,string title,string units,const vector<string>& labels,const vector<double>& values,double maximum,double step,int precision,bool logarithmic=false,string suffix="") {
    SVG s(file,title);
    auto y=[&](double v){return 400-310*(logarithmic?log10(v)/log10(maximum):v/maximum);};
    for(double t=logarithmic?1:0;t<=maximum;t=logarithmic?t*10:t+step){s.line(100,y(t),910,y(t));s.text(88,y(t)+5,number(t,0),14,gray,"end");}
    s.text(100,70,units,15,gray,"start");
    double spacing=810.0/values.size();
    for(size_t i=0;i<values.size();++i){double x=100+spacing*(i+.5), top=y(values[i]); string c=values.size()==2?(i?green:blue):(i==0?gray:i==1?blue:green);s.rect(x-65,top,130,400-top,c);s.text(x,top-10,number(values[i],precision)+suffix,18);s.text(x,430,labels[i],15);}
    if(logarithmic)s.text(480,478,"Nearest-Neighbor time was not reported. SA time is average per run.",15,gray);
}
void annealing(const fs::path& file,const vector<double>& runs,double twoOpt){
    SVG s(file,"Day 1: Simulated Annealing Performance");
    auto y=[](double v){return 400-(v-160)*15.5;};
    for(int t=160;t<=180;t+=5){s.line(100,y(t),910,y(t));s.text(85,y(t)+5,to_string(t),14,gray,"end");}
    s.text(100,70,"Distance (dataset units)",15,gray,"start");
    double avg=accumulate(runs.begin(),runs.end(),0.0)/runs.size();
    s.line(100,y(avg),910,y(avg),gray,2,true);s.line(100,y(twoOpt),910,y(twoOpt),blue,2,true);
    for(size_t i=0;i<runs.size();++i){double x=160+160*i;if(i)s.line(x-160,y(runs[i-1]),x,y(runs[i]),green,3);s.circle(x,y(runs[i]),5,green);s.text(x,y(runs[i])-13,number(runs[i],1));s.text(x,430,"Run "+to_string(i+1));}
    s.text(260,480,"SA average: "+number(avg),16,gray);s.text(680,480,"2-opt: "+number(twoOpt,1),16,blue);
}
void routes(const fs::path& file){
    SVG s(file,"Five-location test case: Before and After");
    vector<pair<double,double>> p={{70,350},{340,160},{340,350},{205,105},{70,160}};
    vector<vector<int>> routes={{0,4,1,2,3,0},{0,4,3,1,2,0}};
    for(int k=0;k<2;++k){double offset=40+480*k;string c=k?green:"#dc2626";s.text(offset+205,75,k?"2-opt: distance 26":"Nearest-Neighbor: distance 33",18);
        for(size_t j=1;j<routes[k].size();++j){auto [x,y]=p[routes[k][j-1]];auto [a,b]=p[routes[k][j]];s.line(x+offset,y,a+offset,b,c,3);double dx=a-x,dy=b-y,len=hypot(dx,dy);dx/=len;dy/=len;double mx=(x+a)/2+offset,my=(y+b)/2;s.line(mx,my,mx-12*dx+6*dy,my-12*dy-6*dx,c,2);s.line(mx,my,mx-12*dx-6*dy,my-12*dy+6*dx,c,2);}
        for(size_t i=0;i<p.size();++i){auto [x,y]=p[i];s.circle(x+offset,y,13,i?gray:"#ea580c");s.text(x+offset,y+5,to_string(i),14,"white");}s.text(offset+70,380,"Depot",14);s.text(offset+205,420,k?"0 - 4 - 3 - 1 - 2 - 0":"0 - 4 - 1 - 2 - 3 - 0",16);}
    s.text(480,462,"Distance reduction: 21.21%",20,green);s.text(480,492,"Illustrative node positions, not geographic coordinates.",15,gray);
}
int main(int argc,char** argv){try{
    fs::path dir=argc>1?argv[1]:".";fs::create_directories(dir);
    vector<string> names={"Nearest-Neighbor","2-opt","SA (best)"};vector<double> distances={206.8,168.7,166.1}, reductions;
    for(double d:distances)reductions.push_back((distances[0]-d)/distances[0]*100);
    bars(dir/"TotalDeliveryRoute.svg","Day 1: Total Delivery Route Distance","Distance (dataset units)",names,distances,250,50,1);
    bars(dir/"RouteOptimizationImprovement.svg","Day 1: Route Optimization Improvement","Distance reduction (%)",names,reductions,25,5,2,false,"%");
    bars(dir/"ExecutionTime.svg","Day 1: Execution Time","Time (ms, logarithmic scale)",{"2-opt","SA (average per run)"},{5.0585,2066.29},10000,0,2,true);
    annealing(dir/"SimulatedAnnealing.svg",{166.1,167.9,174.7,173.0,172.1},168.7);
    routes(dir/"BeforeAfterDelivery.svg");cout<<"Generated five SVG visualizations in "<<dir<<'\n';
}catch(const exception& e){cerr<<e.what()<<'\n';return 1;}return 0;}
