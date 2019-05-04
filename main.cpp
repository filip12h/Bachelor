#include <set>
#include <iostream>
#include "basic_impl.hpp"
#include "io/print_nice.hpp"
#include <invariants.hpp>
#include "redBlackEdges.cpp"


using namespace ba_graph;
using namespace std;

// program kompilujem nasledujucim prikazom
// g++ -std=c++17 -fconcepts main.cpp


int main() {
    Factory f;
    Graph g(createG(f));

    //pozri do nakresov ako tento graf vyzera
    for(int i=0;i<32;i++) addV(g, i, f);
    addE(g, Loc( 0, 1), f);
    addE(g, Loc( 0, 3), f);
    addE(g, Loc( 0, 4), f);
    addE(g, Loc( 1, 2), f);
    addE(g, Loc( 1, 3), f);
    addE(g, Loc( 2, 3), f);
    addE(g, Loc( 2, 4), f);
    addE(g, Loc( 4, 5), f);
    addE(g, Loc( 5, 6), f);
    addE(g, Loc( 5, 31), f);
    addE(g, Loc( 6, 7), f);
    addE(g, Loc( 6, 12), f);
    addE(g, Loc( 7, 8), f);
    addE(g, Loc( 7, 10), f);
    addE(g, Loc( 8, 9), f);
    addE(g, Loc( 8, 11), f);
    addE(g, Loc( 9, 10), f);
    addE(g, Loc( 9, 11), f);
    addE(g, Loc( 10, 11), f);
    addE(g, Loc( 12, 13), f);
    addE(g, Loc( 12, 14), f);
    addE(g, Loc( 13, 14), f);
    addE(g, Loc( 13, 15), f);
    addE(g, Loc( 14, 30), f);
    addE(g, Loc( 15, 16), f);
    addE(g, Loc( 15, 20), f);
    addE(g, Loc( 16, 17), f);
    addE(g, Loc( 16, 18), f);
    addE(g, Loc( 17, 18), f);
    addE(g, Loc( 17, 22), f);
    addE(g, Loc( 18, 19), f);
    addE(g, Loc( 19, 20), f);
    addE(g, Loc( 19, 21), f);
    addE(g, Loc( 20, 21), f);
    addE(g, Loc( 21, 26), f);
    addE(g, Loc( 22, 23), f);
    addE(g, Loc( 22, 24), f);
    addE(g, Loc( 23, 24), f);
    addE(g, Loc( 23, 25), f);
    addE(g, Loc( 24, 25), f);
    addE(g, Loc( 25, 26), f);
    addE(g, Loc( 26, 27), f);
    addE(g, Loc( 27, 28), f);
    addE(g, Loc( 27, 29), f);
    addE(g, Loc( 28, 29), f);
    addE(g, Loc( 28, 31), f);
    addE(g, Loc( 29, 30), f);
    addE(g, Loc( 30, 31), f);

    set<pair<Number, Number>> blackEdges;
    Graph blackGraph(createG(f));
    for (auto &rot: g)
        addV(blackGraph, rot.n().to_int(), f);
    addE(blackGraph, Loc( 0, 4), f);
    addE(blackGraph, Loc( 1, 2), f);
    addE(blackGraph, Loc( 2, 3), f);
    addE(blackGraph, Loc( 5, 6), f);
    addE(blackGraph, Loc( 6, 7), f);
    addE(blackGraph, Loc( 8, 9), f);
    addE(blackGraph, Loc( 9, 10), f);
    addE(blackGraph, Loc( 10, 11), f);
    addE(blackGraph, Loc( 12, 13), f);
    addE(blackGraph, Loc( 12, 14), f);
    addE(blackGraph, Loc( 13, 14), f);
    addE(blackGraph, Loc( 13, 15), f);
    addE(blackGraph, Loc( 14, 30), f);
    addE(blackGraph, Loc( 16, 18), f);
    addE(blackGraph, Loc( 17, 18), f);
    addE(blackGraph, Loc( 18, 19), f);
    addE(blackGraph, Loc( 19, 20), f);
    addE(blackGraph, Loc( 19, 21), f);
    addE(blackGraph, Loc( 20, 21), f);
    addE(blackGraph, Loc( 21, 26), f);
    addE(blackGraph, Loc( 22, 23), f);
    addE(blackGraph, Loc( 22, 24), f);
    addE(blackGraph, Loc( 23, 24), f);
    addE(blackGraph, Loc( 23, 25), f);
    addE(blackGraph, Loc( 24, 25), f);
    addE(blackGraph, Loc( 25, 26), f);
    addE(blackGraph, Loc( 26, 27), f);
    addE(blackGraph, Loc( 27, 28), f);
    addE(blackGraph, Loc( 27, 29), f);
    addE(blackGraph, Loc( 28, 29), f);
    addE(blackGraph, Loc( 28, 31), f);

    for (auto &rot: blackGraph){
        for (auto &inc: rot){
            if (inc.n1()<inc.n2())
                blackEdges.insert(pair(inc.n1(), inc.n2()));
        }
    }

    float epsilon;

    cout<< "Zvol hodnotu epsilon z intervalu (0, 1/2)\n";//pozn: prilis velke alebo male epsilon nam nemusia vyhovovat
    cin >> epsilon;

    assert((g.size()-blackGraph.size())>((1/2+epsilon)*g.order()));

    cout<<blackGraph<<"\n";


    cout<<g<<"\n";


    set<Number> ourSet = redBlackEdges(g, blackEdges, epsilon, blackGraph, f);

    cout<<ourSet;


}