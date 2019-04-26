#include <set>
#include <iostream>
#include <basic_impl.hpp>
#include <io/print_nice.hpp>
#include <invariants.hpp>
#include "redBlackEdges.h"


using namespace ba_graph;
using namespace std;

/*
 * pokus o classu rozsirujucu Graph. Problem s privatnym konstruktorom
 */

//class MyComponent: public Graph{
//public:
//    friend Graph;
//    int numOfDelEdges;
//    bool isLarge;
//    bool isThin;
//    float delta;
//
//
//};

// program kompilujem nasledujucim prikazom
// g++ -std=c++17 -fconcepts main.cpp


int main() {
    Factory f;
    Graph g(createG(f));

    for(int i=0;i<8;i++) addV(g, i, f);
    addV(g, 4, f);
    for(int i=0;i<3;i++) addE(g, Loc(i, i+1), f);
    addE(g, Loc(3,0), f);
    for(int i=4;i<6;i++) addE(g, Loc(i, i+1), f);

    float epsilon;

    cout<< "Zvol hodnotu epsilon z intervalu (0, 1/2)\n";//pozn: prilis velke alebo male epsilon nam nemusia vyhovovat
    cin >> epsilon;


    std::cout<<g;
//    vector<Graph> sg = connectedComponents(g, f);
//
//    while (!sg.empty())
//    {cout<<sg.back();
//    sg.pop_back();}

};