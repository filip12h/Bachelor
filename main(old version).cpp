#include <iostream>
#include "basic_impl.hpp"
#include "io/print_nice.hpp"
#include <invariants.hpp>
#include "cutSizeBisection.cpp"
#include "pathDecomposition.cpp"
#include <cmath>
#include "random/random_graphs.hpp"

using namespace ba_graph;
using namespace std;

int main(){

    Factory f;
    /*
     * here are some examples of graphs
     */
    //TODO: ERROR
    Graph graph = random_regular_multigraph(102, 3);

    Graph g(createG(f));
    for(int i=0;i<32;i++)
        addV(g, i, f);
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

    string answer;
    cout<<"Do you want to use graph from command line? (y/n)";
    cin>>answer;

    assert((answer=="y")||(answer=="n"));
    //Graph graph(createG(f));
    if (answer=="y"){
        cout<<"Set number of vertices in graph\n";
        int numOfVertices;
        cin>>numOfVertices;
        for (int i = 0; i<numOfVertices; i++)
            addV(graph, i);
        cout<<"now set all vertices in format \"u v\", where u and v are vertices which the edge connects\n";
        cout<<"make sure that input graph is 3-regular\n";
        for (int j = 0; j<numOfVertices*3/2; j++){
            cout<<"edge number "<<j<<":";
            int u, v;
            cin>>u>>v;
            addE(graph, Loc(u, v));
        }
    } else {
        cout<<"make sure you have set your graph configuration in main.cpp\n";
        cout<<"graph has to be 3-regular and its name must replace \"graph\" in rows under 4 \"TODO\"'s in main.cpp\n";
        cout<<"for start path decomposition, type any symbol and push enter\n";
        cin>>answer;
    }

    float epsilon = 1;

    //bisectionSet is in ((a, b), (c, d)) format, where b is V0, d is V1 and a(c) are C-vertices in V0(V1)
    long long int bisectionWidth = INT64_MAX;
    pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> bisectionSet;
    while (true) {
        //TODO: set name of your graph as parameter of getGoodBisection
        pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>>
                candidateBisection = getGoodBisection(graph, epsilon, f);
        if (getCutsize(bigGraph, candidateBisection.first.second, candidateBisection.second.second)<bisectionWidth){
            bisectionSet = candidateBisection;
            //TODO: set name of your graph as parameter of getCutSize
            bisectionWidth = getCutsize(graph, candidateBisection.first.second, candidateBisection.second.second);
            //bisectionWidth = max(candidateBisection.first.first.size(), candidateBisection.second.first.size());
        } else {
            //long double limit = (1.0/(pow(graph.order(), 3)));
            //this^ was considered as very low number for some larger graphs
            if (epsilon<0.0000000000001) //every time I get here, it takes epsilon constant steps to det here
                //in case of smaller graphs, we can increse value of limit, which epsilon has to undergo
                break;
        }
        //to achive possibly better bisection we decrease epsilon
        epsilon /= 2;
    }

    //cout<<bisectionSet<<"\n\n";

    Graph v0Graph(createG(f));
    for (auto &n:bisectionSet.first.second)
        addV(v0Graph, n);
    for (auto &n:bisectionSet.first.second)
        //TODO: set your graph's name in for auto loop. for example: yourGraph[n]
        for (auto &n2: graph[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet.first.second.find(n2.n2())!=bisectionSet.first.second.end()))
                addE(v0Graph, Loc(n, n2.n2()));
    Graph v1Graph(createG(f));
    for (auto &n:bisectionSet.second.second)
        addV(v1Graph, n);
    for (auto &n:bisectionSet.second.second)
        //TODO: set your graph's name in for auto loop
        for (auto &n2: graph[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet.second.second.find(n2.n2())!=bisectionSet.second.second.end()))
                addE(v1Graph, Loc(n, n2.n2()));

    vector<set<Number>> v0Decomposition;
    makeDecomposition(v0Graph, v0Decomposition, bisectionSet.first.first, bisectionSet.first.second);
    vector<set<Number>> v1Decomposition;
    makeDecomposition(v1Graph, v1Decomposition, bisectionSet.second.first, bisectionSet.second.second);

    vector<set<Number>> middleDecomposition;
    //TODO: you have to set your graph as parameter to getMiddleDecomposition
    middleDecomposition = getMiddleDecomposition(graph, middleDecomposition, bisectionSet.first.first,
            bisectionSet.second.first);

    vector<set<Number>> decomposition;
    decomposition.insert(decomposition.end(), v0Decomposition.rbegin(), v0Decomposition.rend());
    decomposition.insert(decomposition.end(), middleDecomposition.begin(), middleDecomposition.end());
    decomposition.insert(decomposition.end(), v1Decomposition.begin(), v1Decomposition.end());

    //cout<<decomposition<<"\n";

    int pathWidth = reduceDecomposition(decomposition);

    cout<<"This graph's pathwidth is: "<<pathWidth<<"\n\n";
    cout<<decomposition<<"\n";



//    cout<<"Do you want to test if decomposition went good or wrong? (Y/N)"
//    char answer;
//    cin>>answer;
//    if (answer == "Y"){
//
//    } else {
//
//    }

    //test if decomposition is working well
}