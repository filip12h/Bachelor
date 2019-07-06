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

using namespace ba_graph;
using namespace std;

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

                cout << "Please enter a path of file to read from\n";
                cin >> filename;
                filename = R"(C:\Users\Karol\ClionProjects\Bachelor\graph_examples\)" + filename + ".txt";
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

        cout << R"(OK, you have set your input graph. Now make cut (type "c") or path decomposition (type "d"))"<<"\n";
        cin >> answer;
        if (answer == "c") {
            makeBisection(graph, f, true);
        } else if (answer == "d")
            pathDecomposition(graph, f);
        else cerr << R"(you have entered other than "c" or "d")"<<"\n";

        cout<<"if you want to quit, type \"quit\", otherwise type anything else.\n";
        cin>>answer;
    }

    cout<<"THANKS FOR YOUR TIME, THEN YOU CAN THANK ME FOR MINE :)";

}