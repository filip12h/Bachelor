//#include <set>
#include <iostream>
#include "basic_impl.hpp"
#include "io/print_nice.hpp"
#include <invariants.hpp>
#include "cutSizeBisection.cpp"
#include "pathDecomposition.cpp"
//#include "gtest/gtest.h"
//#include "gmock/gmock.h"


using namespace ba_graph;
using namespace std;

// program kompilujem nasledujucim prikazom
// g++ -std=c++17 -fconcepts main.cpp


int main(){
//int main(int argc, char* argv[]) {
//    testing::InitGoogleTest(&argc, argv);
//    RUN_ALL_TESTS();

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

    //cout<<blackGraph<<"\n";


    //cout<<g<<"\n";

    Graph myGraph(createG(f));
    for (int i = 0; i<16; i++)
        addV(myGraph, i, f);
    addE(myGraph, Loc( 0,1 ));
    addE(myGraph, Loc( 0,9 ));
    addE(myGraph, Loc( 0,11 ));
    addE(myGraph, Loc( 1,2 ));
    addE(myGraph, Loc( 1,10 ));
    addE(myGraph, Loc( 2,3 ));
    addE(myGraph, Loc( 2,14 ));
    addE(myGraph, Loc( 3,4 ));
    addE(myGraph, Loc( 3,13 ));
    addE(myGraph, Loc( 4,5 ));
    addE(myGraph, Loc( 4,13 ));
    addE(myGraph, Loc( 5,6 ));
    addE(myGraph, Loc( 5,14 ));
    addE(myGraph, Loc( 6,7 ));
    addE(myGraph, Loc( 6,15 ));
    addE(myGraph, Loc( 7,8 ));
    addE(myGraph, Loc( 7,15 ));
    addE(myGraph, Loc( 8,9 ));
    addE(myGraph, Loc( 8,15 ));
    addE(myGraph, Loc( 9,10 ));
    addE(myGraph, Loc( 10,11 ));
    addE(myGraph, Loc( 11,12 ));
    addE(myGraph, Loc( 12,13 ));
    addE(myGraph, Loc( 12,14 ));

    Graph symethricGraph(createG(f));

    for (int i=0; i<10; i++)
        addV(symethricGraph, i);
    addE(symethricGraph, Loc( 0,2 ));
    addE(symethricGraph, Loc( 0,7 ));
    addE(symethricGraph, Loc( 0,9 ));
    addE(symethricGraph, Loc( 1,6 ));
    addE(symethricGraph, Loc( 1,8 ));
    addE(symethricGraph, Loc( 1,9 ));
    addE(symethricGraph, Loc( 2,4 ));
    addE(symethricGraph, Loc( 2,7 ));
    addE(symethricGraph, Loc( 3,5 ));
    addE(symethricGraph, Loc( 3,6 ));
    addE(symethricGraph, Loc( 3,8 ));
    addE(symethricGraph, Loc( 4,7 ));
    addE(symethricGraph, Loc( 4,9 ));
    addE(symethricGraph, Loc( 5,6 ));
    addE(symethricGraph, Loc( 5,8 ));

    Graph koliskaGraph(createG(f));

    for (int i=0; i<14; i++)
        addV(koliskaGraph, i);
    addE(koliskaGraph, Loc( 0,11 ));
    addE(koliskaGraph, Loc( 0,12 ));
    addE(koliskaGraph, Loc( 0,13 ));
    addE(koliskaGraph, Loc( 1,7 ));
    addE(koliskaGraph, Loc( 1,10 ));
    addE(koliskaGraph, Loc( 1,12 ));
    addE(koliskaGraph, Loc( 2,6 ));
    addE(koliskaGraph, Loc( 2,8 ));
    addE(koliskaGraph, Loc( 2,10 ));
    addE(koliskaGraph, Loc( 3,11 ));
    addE(koliskaGraph, Loc( 3,12 ));
    addE(koliskaGraph, Loc( 3,13 ));
    addE(koliskaGraph, Loc( 4,5 ));
    addE(koliskaGraph, Loc( 4,6 ));
    addE(koliskaGraph, Loc( 4,9 ));
    addE(koliskaGraph, Loc( 5,7 ));
    addE(koliskaGraph, Loc( 5,9 ));
    addE(koliskaGraph, Loc( 6,8 ));
    addE(koliskaGraph, Loc( 7,10 ));
    addE(koliskaGraph, Loc( 8,9 ));
    addE(koliskaGraph, Loc( 11,13 ));

    Graph bigGraph(createG(f));

    for (int i=0; i<32; i++)
        addV(bigGraph, i);
    addE(bigGraph, Loc( 0,1 ));
    addE(bigGraph, Loc( 0,15 ));
    addE(bigGraph, Loc( 0,16 ));
    addE(bigGraph, Loc( 1,2 ));
    addE(bigGraph, Loc( 1,17 ));
    addE(bigGraph, Loc( 2,3 ));
    addE(bigGraph, Loc( 2,18 ));
    addE(bigGraph, Loc( 3,4 ));
    addE(bigGraph, Loc( 3,20 ));
    addE(bigGraph, Loc( 4,5 ));
    addE(bigGraph, Loc( 4,21 ));
    addE(bigGraph, Loc( 5,6 ));
    addE(bigGraph, Loc( 5,22 ));
    addE(bigGraph, Loc( 6,7 ));
    addE(bigGraph, Loc( 6,23 ));
    addE(bigGraph, Loc( 7,8 ));
    addE(bigGraph, Loc( 7,24 ));
    addE(bigGraph, Loc( 8,9 ));
    addE(bigGraph, Loc( 8,24 ));
    addE(bigGraph, Loc( 9,10 ));
    addE(bigGraph, Loc( 9,26 ));
    addE(bigGraph, Loc( 10,11 ));
    addE(bigGraph, Loc( 10,26 ));
    addE(bigGraph, Loc( 11,12 ));
    addE(bigGraph, Loc( 11,30 ));
    addE(bigGraph, Loc( 12,13 ));
    addE(bigGraph, Loc( 12,27 ));
    addE(bigGraph, Loc( 13,14 ));
    addE(bigGraph, Loc( 13,27 ));
    addE(bigGraph, Loc( 14,15 ));
    addE(bigGraph, Loc( 14,28 ));
    addE(bigGraph, Loc( 15,16 ));
    addE(bigGraph, Loc( 16,28 ));
    addE(bigGraph, Loc( 17,18 ));
    addE(bigGraph, Loc( 17,29 ));
    addE(bigGraph, Loc( 18,29 ));
    addE(bigGraph, Loc( 19,20 ));
    addE(bigGraph, Loc( 19,27 ));
    addE(bigGraph, Loc( 19,31 ));
    addE(bigGraph, Loc( 20,21 ));
    addE(bigGraph, Loc( 21,31 ));
    addE(bigGraph, Loc( 22,23 ));
    addE(bigGraph, Loc( 22,25 ));
    addE(bigGraph, Loc( 23,25 ));
    addE(bigGraph, Loc( 24,25 ));
    addE(bigGraph, Loc( 26,30 ));
    addE(bigGraph, Loc( 28,29 ));
    addE(bigGraph, Loc( 30,31 ));


    pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>>
            bisectionSet = getGoodBisection(bigGraph, epsilon, f);


//    set<Number> ourSet = redBlackEdges(g, blackEdges, epsilon, blackGraph, f);
//
//    cout<<ourSet<<"\n";

    cout<<bisectionSet;

    Graph v0Graph(createG(f));
    for (auto &n:bisectionSet.first.second)
        addV(v0Graph, n);
    for (auto &n:bisectionSet.first.second)
        for (auto &n2: bigGraph[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet.first.second.find(n2.n2())!=bisectionSet.first.second.end()))
                addE(v0Graph, Loc(n, n2.n2()));
    Graph v1Graph(createG(f));
    for (auto &n:bisectionSet.second.second)
        addV(v1Graph, n);
    for (auto &n:bisectionSet.second.second)
        for (auto &n2: bigGraph[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet.second.second.find(n2.n2())!=bisectionSet.second.second.end()))
                addE(v1Graph, Loc(n, n2.n2()));

    vector<set<Number>> v0Decomposition;
    makeDecomposition(v0Graph, v0Decomposition, bisectionSet.first.first, bisectionSet.first.second);
    vector<set<Number>> v1Decomposition;
    makeDecomposition(v1Graph, v1Decomposition, bisectionSet.second.first, bisectionSet.second.second);

    vector<set<Number>> middleDecomposition = getMiddleDecomposition(bigGraph, middleDecomposition, bisectionSet.first.first,
            bisectionSet.second.first);

    cout<<v0Decomposition<<"\n";

    cout<<v1Decomposition<<"\n";

    cout<<middleDecomposition<<"\n";


}