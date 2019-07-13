#include <iostream>
#include <fstream>
#include "basic_impl.hpp"
#include "pathDecomposition.hpp"

using namespace ba_graph;
using namespace std;

Graph open(string filename, Factory &f){
    ifstream file;
    Graph graph(createG(f));
    file.open(filename);//.c_str() );
    filename = "C:\\Users\\Karol\\ClionProjects\\Bachelor\\graph_examples\\" + filename + ".txt";
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
    return graph;
}

bool bisectionTest(Graph &graph, Factory &f, int epsilonExp){

    float epsilonLimit = pow(0.1, epsilonExp);

    float epsilon = 1.0/2;

    long long int bisectionWidth = INT64_MAX;
    pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> bisectionSet;

    while (true) {
        pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>>
                candidateBisection = getGoodBisection(graph, epsilon, f);
        if (getCutsize(graph, candidateBisection.first.second, candidateBisection.second.second)<bisectionWidth){
            bisectionSet = candidateBisection;
            bisectionWidth = getCutsize(graph, candidateBisection.first.second, candidateBisection.second.second);
        } else
        if (epsilon<=epsilonLimit) break;
        epsilon /= 2;
    }
    for (auto n: bisectionSet.first.first){
        if (bisectionSet.first.second.find(n)==bisectionSet.first.second.end()) return false;
        if (bisectionSet.second.second.find(n)!=bisectionSet.second.second.end()) return false;
        if (bisectionSet.second.first.find(n)!=bisectionSet.second.first.end()) return false;
    }
    for (auto n: bisectionSet.second.first){
        if (bisectionSet.second.second.find(n)==bisectionSet.second.second.end()) return false;
        if (bisectionSet.first.second.find(n)!=bisectionSet.first.second.end()) return false;
        if (bisectionSet.first.first.find(n)!=bisectionSet.first.first.end()) return false;
    }
    return true;
}

bool pathDecompositionTest(Graph &graph, Factory &f, int epsilonExp){
    pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> bisectionSet = makeBisection(graph, f, epsilonExp);

    Graph v0Graph(createG(f));
    for (auto &n:bisectionSet.first.second)
        addV(v0Graph, n, f);
    for (auto &n:bisectionSet.first.second)
        for (auto &n2: graph[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet.first.second.find(n2.n2())!=bisectionSet.first.second.end()))
                addE(v0Graph, Loc(n, n2.n2()), f);
    Graph v1Graph(createG(f));
    for (auto &n:bisectionSet.second.second)
        addV(v1Graph, n, f);
    for (auto &n:bisectionSet.second.second)
        for (auto &n2: graph[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet.second.second.find(n2.n2())!=bisectionSet.second.second.end()))
                addE(v1Graph, Loc(n, n2.n2()), f);

    vector<set<Number>> v0Decomposition;
    makeDecomposition(v0Graph, v0Decomposition, bisectionSet.first.first, bisectionSet.first.second);
    vector<set<Number>> v1Decomposition;
    makeDecomposition(v1Graph, v1Decomposition, bisectionSet.second.first, bisectionSet.second.second);

    vector<set<Number>> middleDecomposition;
    middleDecomposition = getMiddleDecomposition(graph, middleDecomposition, bisectionSet.first.first,
                                                 bisectionSet.second.first);

    vector<set<Number>> decomposition;
    decomposition.insert(decomposition.end(), v0Decomposition.rbegin(), v0Decomposition.rend());
    decomposition.insert(decomposition.end(), middleDecomposition.begin(), middleDecomposition.end());
    decomposition.insert(decomposition.end(), v1Decomposition.begin(), v1Decomposition.end());

    //cout<<decomposition<<"\n";

    int sizeOfDecomposition = reduceDecomposition(decomposition);

    return decompositionTest(graph, decomposition);

}

int main(){
    Factory f;
    Graph g1(createG(f)), g2(createG(f)), g3(createG(f)), g4(createG(f)), g5(createG(f)),
            g6(createG(f)), g7(createG(f));
    g1 = open("bigGraph", f);
    g2 = open("graph", f);
    g3 = open("koliskaGraph", f);
    g4 = open("myGraph", f);
    g5 = open("petersen", f);
    g6 = open("snarkJ9", f);
    g7 = open("symetricGraph", f);

    for (int i = 1; i<20; i = i+7){
        assert(pathDecompositionTest(g1, f, i));
        assert(pathDecompositionTest(g2, f, i));
        assert(pathDecompositionTest(g3, f, i));
        assert(pathDecompositionTest(g4, f, i));
        assert(pathDecompositionTest(g5, f, i));
        assert(pathDecompositionTest(g6, f, i));
        assert(pathDecompositionTest(g7, f, i));
        assert(bisectionTest(g1, f, i));
        assert(bisectionTest(g2, f, i));
        assert(bisectionTest(g3, f, i));
        assert(bisectionTest(g4, f, i));
        assert(bisectionTest(g5, f, i));
        assert(bisectionTest(g6, f, i));
        assert(bisectionTest(g7, f, i));
    }

    cout << "END OF TEST";

}

