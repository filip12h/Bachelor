#include <iostream>
#include "basic_impl.hpp"
#include "io/print_nice.hpp"
#include "random/random_graphs.hpp"
#include <invariants.hpp>
#include "cutSizeBisection.cpp"
#include "pathDecomposition.cpp"
#include <cmath>
#include <map>

using namespace ba_graph;
using namespace std;

int main(){
    
    Factory f;
    /*
     * here are some examples of graphs
     */
    Graph petersen(createG(f));
    for(int i=0;i<10;i++)
        addV(petersen, i, f);
    addE(petersen, Loc( 0, 2), f);
    addE(petersen, Loc( 0, 3), f);
    addE(petersen, Loc( 0, 5), f);
    addE(petersen, Loc( 1, 3), f);
    addE(petersen, Loc( 1, 4), f);
    addE(petersen, Loc( 1, 6), f);
    addE(petersen, Loc( 2, 4), f);
    addE(petersen, Loc( 2, 7), f);
    addE(petersen, Loc( 3, 8), f);
    addE(petersen, Loc( 4, 9), f);
    addE(petersen, Loc( 5, 6), f);
    addE(petersen, Loc( 5, 9), f);
    addE(petersen, Loc( 6, 7), f);
    addE(petersen, Loc( 7, 8), f);
    addE(petersen, Loc( 8, 9), f);

    Graph snarkJ9(createG(f));
    for(int i=0;i<36;i++)
        addV(snarkJ9, i, f);
    addE(snarkJ9, Loc( 0, 1), f);
    addE(snarkJ9, Loc( 0, 26), f);
    addE(snarkJ9, Loc( 0, 27), f);
    addE(snarkJ9, Loc( 1, 2), f);
    addE(snarkJ9, Loc( 1, 23), f);
    addE(snarkJ9, Loc( 2, 3), f);
    addE(snarkJ9, Loc( 2, 7), f);
    addE(snarkJ9, Loc( 3, 4), f);
    addE(snarkJ9, Loc( 3, 28), f);
    addE(snarkJ9, Loc( 4, 5), f);
    addE(snarkJ9, Loc( 4, 26), f);
    addE(snarkJ9, Loc( 5, 6), f);
    addE(snarkJ9, Loc( 5, 10), f);
    addE(snarkJ9, Loc( 6, 7), f);
    addE(snarkJ9, Loc( 6, 29), f);
    addE(snarkJ9, Loc( 7, 8), f);
    addE(snarkJ9, Loc( 8, 9), f);
    addE(snarkJ9, Loc( 8, 13), f);
    addE(snarkJ9, Loc( 9, 10), f);
    addE(snarkJ9, Loc( 9, 30), f);
    addE(snarkJ9, Loc( 10, 11), f);
    addE(snarkJ9, Loc( 11, 12), f);
    addE(snarkJ9, Loc( 11, 16), f);
    addE(snarkJ9, Loc( 12, 13), f);
    addE(snarkJ9, Loc( 12, 31), f);
    addE(snarkJ9, Loc( 13, 14), f);
    addE(snarkJ9, Loc( 14, 15), f);
    addE(snarkJ9, Loc( 14, 19), f);
    addE(snarkJ9, Loc( 15, 16), f);
    addE(snarkJ9, Loc( 15, 32), f);
    addE(snarkJ9, Loc( 16, 17), f);
    addE(snarkJ9, Loc( 17, 18), f);
    addE(snarkJ9, Loc( 17, 22), f);
    addE(snarkJ9, Loc( 18, 19), f);
    addE(snarkJ9, Loc( 18, 33), f);
    addE(snarkJ9, Loc( 19, 20), f);
    addE(snarkJ9, Loc( 20, 21), f);
    addE(snarkJ9, Loc( 20, 25), f);
    addE(snarkJ9, Loc( 21, 22), f);
    addE(snarkJ9, Loc( 21, 34), f);
    addE(snarkJ9, Loc( 22, 23), f);
    addE(snarkJ9, Loc( 23, 24), f);
    addE(snarkJ9, Loc( 24, 25), f);
    addE(snarkJ9, Loc( 24, 35), f);
    addE(snarkJ9, Loc( 25, 26), f);
    addE(snarkJ9, Loc( 27, 28), f);
    addE(snarkJ9, Loc( 27, 35), f);
    addE(snarkJ9, Loc( 28, 29), f);
    addE(snarkJ9, Loc( 29, 30), f);
    addE(snarkJ9, Loc( 30, 31), f);
    addE(snarkJ9, Loc( 31, 32), f);
    addE(snarkJ9, Loc( 32, 33), f);
    addE(snarkJ9, Loc( 33, 34), f);
    addE(snarkJ9, Loc( 34, 35), f);


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
    addE(myGraph, Loc( 0,1 ), f);
    addE(myGraph, Loc( 0,9 ), f);
    addE(myGraph, Loc( 0,11 ), f);
    addE(myGraph, Loc( 1,2 ), f);
    addE(myGraph, Loc( 1,10 ), f);
    addE(myGraph, Loc( 2,3 ), f);
    addE(myGraph, Loc( 2,14 ), f);
    addE(myGraph, Loc( 3,4 ), f);
    addE(myGraph, Loc( 3,13 ), f);
    addE(myGraph, Loc( 4,5 ), f);
    addE(myGraph, Loc( 4,13 ), f);
    addE(myGraph, Loc( 5,6 ), f);
    addE(myGraph, Loc( 5,14 ), f);
    addE(myGraph, Loc( 6,7 ), f);
    addE(myGraph, Loc( 6,15 ), f);
    addE(myGraph, Loc( 7,8 ), f);
    addE(myGraph, Loc( 7,15 ), f);
    addE(myGraph, Loc( 8,9 ), f);
    addE(myGraph, Loc( 8,15 ), f);
    addE(myGraph, Loc( 9,10 ), f);
    addE(myGraph, Loc( 10,11 ), f);
    addE(myGraph, Loc( 11,12 ), f);
    addE(myGraph, Loc( 12,13 ), f);
    addE(myGraph, Loc( 12,14 ), f);

    Graph symethricGraph(createG(f));
    for (int i=0; i<10; i++)
        addV(symethricGraph, i, f);
    addE(symethricGraph, Loc( 0,2 ), f);
    addE(symethricGraph, Loc( 0,7 ), f);
    addE(symethricGraph, Loc( 0,9 ), f);
    addE(symethricGraph, Loc( 1,6 ), f);
    addE(symethricGraph, Loc( 1,8 ), f);
    addE(symethricGraph, Loc( 1,9 ), f);
    addE(symethricGraph, Loc( 2,4 ), f);
    addE(symethricGraph, Loc( 2,7 ), f);
    addE(symethricGraph, Loc( 3,5 ), f);
    addE(symethricGraph, Loc( 3,6 ), f);
    addE(symethricGraph, Loc( 3,8 ), f);
    addE(symethricGraph, Loc( 4,7 ), f);
    addE(symethricGraph, Loc( 4,9 ), f);
    addE(symethricGraph, Loc( 5,6 ), f);
    addE(symethricGraph, Loc( 5,8 ), f);

    Graph koliskaGraph(createG(f));
    for (int i=0; i<14; i++)
        addV(koliskaGraph, i, f);
    addE(koliskaGraph, Loc( 0,11 ), f);
    addE(koliskaGraph, Loc( 0,12 ), f);
    addE(koliskaGraph, Loc( 0,13 ), f);
    addE(koliskaGraph, Loc( 1,7 ), f);
    addE(koliskaGraph, Loc( 1,10 ), f);
    addE(koliskaGraph, Loc( 1,12 ), f);
    addE(koliskaGraph, Loc( 2,6 ), f);
    addE(koliskaGraph, Loc( 2,8 ), f);
    addE(koliskaGraph, Loc( 2,10 ), f);
    addE(koliskaGraph, Loc( 3,11 ), f);
    addE(koliskaGraph, Loc( 3,12 ), f);
    addE(koliskaGraph, Loc( 3,13 ), f);
    addE(koliskaGraph, Loc( 4,5 ), f);
    addE(koliskaGraph, Loc( 4,6 ), f);
    addE(koliskaGraph, Loc( 4,9 ), f);
    addE(koliskaGraph, Loc( 5,7 ), f);
    addE(koliskaGraph, Loc( 5,9 ), f);
    addE(koliskaGraph, Loc( 6,8 ), f);
    addE(koliskaGraph, Loc( 7,10 ), f);
    addE(koliskaGraph, Loc( 8,9 ), f);
    addE(koliskaGraph, Loc( 11,13 ), f);

    Graph bigGraph(createG(f));

    for (int i=0; i<32; i++)
        addV(bigGraph, i, f);
    addE(bigGraph, Loc( 0,1 ), f);
    addE(bigGraph, Loc( 0,15 ), f);
    addE(bigGraph, Loc( 0,16 ), f);
    addE(bigGraph, Loc( 1,2 ), f);
    addE(bigGraph, Loc( 1,17 ), f);
    addE(bigGraph, Loc( 2,3 ), f);
    addE(bigGraph, Loc( 2,18 ), f);
    addE(bigGraph, Loc( 3,4 ), f);
    addE(bigGraph, Loc( 3,20 ), f);
    addE(bigGraph, Loc( 4,5 ), f);
    addE(bigGraph, Loc( 4,21 ), f);
    addE(bigGraph, Loc( 5,6 ), f);
    addE(bigGraph, Loc( 5,22 ), f);
    addE(bigGraph, Loc( 6,7 ), f);
    addE(bigGraph, Loc( 6,23 ), f);
    addE(bigGraph, Loc( 7,8 ), f);
    addE(bigGraph, Loc( 7,24 ), f);
    addE(bigGraph, Loc( 8,9 ), f);
    addE(bigGraph, Loc( 8,24 ), f);
    addE(bigGraph, Loc( 9,10 ), f);
    addE(bigGraph, Loc( 9,26 ), f);
    addE(bigGraph, Loc( 10,11 ), f);
    addE(bigGraph, Loc( 10,26 ), f);
    addE(bigGraph, Loc( 11,12 ), f);
    addE(bigGraph, Loc( 11,30 ), f);
    addE(bigGraph, Loc( 12,13 ), f);
    addE(bigGraph, Loc( 12,27 ), f);
    addE(bigGraph, Loc( 13,14 ), f);
    addE(bigGraph, Loc( 13,27 ), f);
    addE(bigGraph, Loc( 14,15 ), f);
    addE(bigGraph, Loc( 14,28 ), f);
    addE(bigGraph, Loc( 15,16 ), f);
    addE(bigGraph, Loc( 16,28 ), f);
    addE(bigGraph, Loc( 17,18 ), f);
    addE(bigGraph, Loc( 17,29 ), f);
    addE(bigGraph, Loc( 18,29 ), f);
    addE(bigGraph, Loc( 19,20 ), f);
    addE(bigGraph, Loc( 19,27 ), f);
    addE(bigGraph, Loc( 19,31 ), f);
    addE(bigGraph, Loc( 20,21 ), f);
    addE(bigGraph, Loc( 21,31 ), f);
    addE(bigGraph, Loc( 22,23 ), f);
    addE(bigGraph, Loc( 22,25 ), f);
    addE(bigGraph, Loc( 23,25 ), f);
    addE(bigGraph, Loc( 24,25 ), f);
    addE(bigGraph, Loc( 26,30 ), f);
    addE(bigGraph, Loc( 28,29 ), f);
    addE(bigGraph, Loc( 30,31 ), f);

    string answer;
    cout<<"Do you want to use graph from command line? (y/n)";
    cin>>answer;

    assert((answer=="y")||(answer=="n"));
    Graph graph(createG(f));
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
        cout<<"graph has to be 3-regular and its name must be under \"TODO\" row in main.cpp\n";
        cout<<"for start path decomposition, type any symbol and push enter\n";
        cin>>answer;
    }

    float epsilon = 1;

    //bisectionSet is in ((a, b), (c, d)) format, where b is V0, d is V1 and a(c) are C-vertices in V0(V1)
    long long int bisectionWidth = INT64_MAX;
    pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> bisectionSet;

    //TODO: under this line set the graph you want to use. For example: Graph graph_calculate = std::move(bigGraph);
    // Graph graph_calculate = random_regular_multigraph(100, 3, f);

    Graph graph_calculate = random_regular_multigraph(200, 3, f);

    while (true) {
        pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>>
                candidateBisection = getGoodBisection(graph_calculate, epsilon, f);
        if (getCutsize(graph_calculate, candidateBisection.first.second, candidateBisection.second.second)<bisectionWidth){
            bisectionSet = candidateBisection;
            bisectionWidth = getCutsize(graph_calculate, candidateBisection.first.second, candidateBisection.second.second);
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
        addV(v0Graph, n, f);
    for (auto &n:bisectionSet.first.second)
        for (auto &n2: graph_calculate[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet.first.second.find(n2.n2())!=bisectionSet.first.second.end()))
                addE(v0Graph, Loc(n, n2.n2()), f);
    Graph v1Graph(createG(f));
    for (auto &n:bisectionSet.second.second)
        addV(v1Graph, n, f);
    for (auto &n:bisectionSet.second.second)
        for (auto &n2: graph_calculate[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet.second.second.find(n2.n2())!=bisectionSet.second.second.end()))
                addE(v1Graph, Loc(n, n2.n2()), f);

    vector<set<Number>> v0Decomposition;
    makeDecomposition(v0Graph, v0Decomposition, bisectionSet.first.first, bisectionSet.first.second);
    vector<set<Number>> v1Decomposition;
    makeDecomposition(v1Graph, v1Decomposition, bisectionSet.second.first, bisectionSet.second.second);

    vector<set<Number>> middleDecomposition;
    middleDecomposition = getMiddleDecomposition(graph_calculate, middleDecomposition, bisectionSet.first.first,
            bisectionSet.second.first);

    vector<set<Number>> decomposition;
    decomposition.insert(decomposition.end(), v0Decomposition.rbegin(), v0Decomposition.rend());
    decomposition.insert(decomposition.end(), middleDecomposition.begin(), middleDecomposition.end());
    decomposition.insert(decomposition.end(), v1Decomposition.begin(), v1Decomposition.end());

    //cout<<decomposition<<"\n";

    int sizeOfDecomposition = reduceDecomposition(decomposition);

    cout<<"Size of the path decomposition is: "<<sizeOfDecomposition<<"\n\n";
    cout<<decomposition<<"\n";


    cout<<"Do you want to test if decomposition went good or wrong? (y/n)";
    cin>>answer;
    if (answer == "y") {
        //test if decomposition is working well.....but test is itself BAD... :-(
        /*
         * first phase
         */
        bool goodDecomposition = true;
        //every edge is unchecked at the beginning, we caount number of checked edges
        int num_of_checked = 0;
        map<int,bool>checked;
        for (auto &rot:graph_calculate)
            for (auto &edge:rot)
                checked[edge.e().to_int()] = false;
        //we check every bag. in bag for every vertex we search for its neighbor in certain bag, then we check edge
        for (auto &bag: decomposition)
            for (auto &v: bag){
                for(auto &neighbor: graph_calculate[v]){
                    if ((bag.find(neighbor.n2())!=bag.end())&&(!checked[neighbor.e().to_int()])){ //not sure what numbers edges have
                        checked[neighbor.e().to_int()] = true;
                        num_of_checked++;
                    }
                }
            }
        if (num_of_checked!=graph_calculate.size()) goodDecomposition = false;
        /*
         * second phase
         */
        for (auto &rot: graph_calculate){
            int q = 0;
            for (auto &bag: decomposition){
                if ((bag.find(rot.n())!=bag.end())&&(q==0)) q++;
                else if ((bag.find(rot.n())==bag.end())&&(q==1)) q++;
                else if ((bag.find(rot.n())!=bag.end())&&(q==2)) goodDecomposition = false;
            }
        }
        if (goodDecomposition) cout<<"Decomposition is GOOD";
        else cout<<"Decomposition is NOT GOOD";
    }
}