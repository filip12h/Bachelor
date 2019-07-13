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
#include <windows.h>

using namespace ba_graph;
using namespace std;

string getexepath()
{
    char result[ MAX_PATH ];
    return std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );
}

int main(){
    
    Factory f;

    string answer = "";
    Graph graph(createG(f));
    while(answer!="quit") {
        bool useNewGraph = true;
        if (answer!=""){
            cout<<"Do you want to use previous graph?(y/n)\n";
            cin>>answer;
            if (answer=="y") useNewGraph = false;
        }
        if (useNewGraph) {
            cout << "if generate random cubic graph, type \"g\"\n";
            cout << "if put graph from file, type \"f\"\n";
            cout << "if put graph from command line, type \"c\"\n";
            cin >> answer;

            assert((answer == "g") || (answer == "f") || (answer == "c"));
            if (answer == "g") {
                cout << "set number of vertices (number has to be even)\n";
                int numOfV;
                cin >> numOfV;
                assert((numOfV > 0) && (numOfV % 2 == 0));
                graph = random_regular_multigraph(numOfV, 3, f);
                cout << "do you want to print generated graph?(y/n)\n";
                cin>>answer;
                if (answer=="y")
                    cout<<graph;
                else if (answer!="n") cout << "answer:\"n\"\n";
            } else if (answer == "c") {
                cout << "Set number of vertices in graph\n";
                int numOfVertices;
                cin >> numOfVertices;
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
            } else if (answer == "f") {
                ifstream file;
                string filename;

                cout << "Please enter a name of file to read from:\n";
                cin >> filename;
                filename = getexepath()+ "\\" + filename + ".txt";
                file.open(filename);//.c_str() );
                if (!file) {
                    cerr << "Unable to open file at path " + filename + "\n";
                    exit(1);
                }
                int x, y;
                if (file >> x) {
                    for (int i = 0; i < x; i++)
                        addV(graph, i, f);
                }
                while (file >> x) {
                    if (file >> y)
                        addE(graph, Loc(x, y), f);
                }
                file.close();
            } else cerr << R"(you have entered other than "g", "f" or "c")" << "\n";
        }

        cout << R"(OK, you have set your input graph. Now make a cut, path decomposition, find one-helpful set or
                find set in red-black graph(d/c/o/r))"<<"\n";
        cin >> answer;
        if (answer == "d")
            pathDecomposition(graph, f);
        else if (answer == "c") {
            pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> bisectionSet  = makeBisection(graph, f);
            cout << "Cut has been done. Its size is: "<<getCutsize(graph, bisectionSet.first.first, bisectionSet.second.first)<<
                 "\nVertices are divided into 2 sets - V0 and V1.\nV0 cut vertices:("<< bisectionSet.first.first.size() << " elements)\n"
                 << bisectionSet.first.first << "\nV1 cut vertices:(" << bisectionSet.second.first.size() << " elements)\n"
                 << bisectionSet.second.first;
            cout << "\nV0 elements:\n" << bisectionSet.first.second << "\nV1 elements:\n" << bisectionSet.second.second<<"\n";
        } else if (answer == "o"){
            cout<<"Set vertices in set V0 and in set V1.\nIn first row write V0-vertices' numbers separated by space."
                  " Non-mentioned vertices will be considered as V1-vertices.\n";
            cin>>answer;
            set<Number> v0set;
            v0set.insert(answer.front());
            cin>>answer;
            cout<<"Set epsilon value. Make sure that cutsize is greater than (1/3 + 2 epsilon)|V0|\n";
            float epsilon;
            cin>>epsilon;
            if ((1.0/3+2*epsilon)*v0set.size()<getCutSize(graph, v0set))
                cout<<getHelpfulSet(graph, v0set, epsilon, f);
            else cerr<<"Bad value of epsilon.\n";
        } else if (answer == "r"){
            cout<<"Now set how many vertices will be black. Every next row will contain one edge = pair of vertices.\n"
                  "Order of vertices should be same as defined. At the end enter \"end\".\n"
                  "MAKE SURE EVERY VERTEX IS COVERED BY BLACK EDGE\n";
            multiset<pair<Number, Number>> blackEdges;
            int n, v, u;
            cin>>n;
            for (int i = 0; i<n; i++){
                cin>>v>>u;
                blackEdges.insert(pair(v, u));
            }
            Graph blackGraph = createG(f);
            for (auto &ver:graph)
                addV(blackGraph, ver.n(), f);
            for (auto &edg:blackEdges)
                addE(blackGraph, Loc(edg.first, edg.second), f);
            float epsilon;
            cout<<"Set value of epsilon.\n"
                  "Number of red edges should be greater than (1/2+epsilon)|V|";
            cin>>epsilon;
            if (graph.size()-blackEdges.size()>(1.0/2+epsilon)*graph.order())
                cout<<redBlackEdges(graph, blackEdges, epsilon, blackGraph, f);
            else cerr<<"Bad value of epsilon.\n";
        } else {
            cout << "Unexpected symbol. The decomposition will be done.\n";
            pathDecomposition(graph, f);
        }

        cout<<"if you want to quit, type \"quit\", otherwise type anything else.\n";
        cin>>answer;
    }

    cout<<"THANKS FOR YOUR TIME, THEN YOU CAN THANK ME FOR MINE :)";

}