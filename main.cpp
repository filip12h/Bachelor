#include <iostream>
#include "basic_impl.hpp"
#include "io/print_nice.hpp"
#include "random/random_graphs.hpp"
#include "pathDecomposition.hpp"
#include <invariants.hpp>
#include <cmath>
#include <map>
#include <fstream>
#include <dirent.h>
#include <string>

using namespace ba_graph;
using namespace std;

int main(){
    
    Factory f;
    string answer = "";
    int numOfVertices;
    ofstream file;
    while(answer!="quit"){
        Graph graph(createG(f));
        cout << "Set number of vertices in graph\n";
        cin >> numOfVertices;
        if (numOfVertices<20)
            cerr << "Number of vertices has to be at least 20. The program may not work well with small inputs.";
//        graph = random_regular_multigraph(numOfVertices, 3, f);
//        cout<<graph<<"\n";
        for (int i = 0; i < numOfVertices; i++)
            addV(graph, i, f);
        cout << "now set all vertices in format \"u v\", where u and v are vertices which the edge connects\n";
        cout << "make sure that input graph is 3-regular\n";
        for (int j = 0; j < numOfVertices * 3 / 2; j++) {
            cout << "edge number " << j << ":";
            int u, v;
            cin >> u >> v;
            addE(graph, Loc(u, v), f);
        }
        cout << "OK, you have set your input graph. Here is the decomposition: (if larger input, wait a while)\n";
        cout << pathDecomposition(graph)<<"\n";
        vector<set<Number>> pd = pathDecomposition(graph);
        file.open("graph_examples/results"+to_string(numOfVertices)+".txt");
        file<< "width of this decomposition is: "<<decompositionWidth(pd)<<"\n";
        file.close();
        cout<<"if you want to quit, type \"quit\", otherwise type anything else.\n";
        cin>>answer;
    }
    cout<<"THANKS FOR YOUR TIME, THEN YOU CAN THANK ME FOR MINE :)";
}