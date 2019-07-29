#include <iostream>
#include <fstream>
#include "basic_impl.hpp"
#include "pathDecomposition.hpp"
#include "io/print_nice.hpp"

using namespace ba_graph;
using namespace std;

//helpful function for tests
Graph open(string filename, Factory &f){
    ifstream file;
    Graph graph(createG(f));
    filename = "graph_examples/" + filename + ".txt";
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
    return graph;
}

//test if cut is good (cut-vertices are subset of set-vertices)
bool bisectionTest(Graph &graph, Factory &f, int epsilonExp){

    float epsilonLimit = pow(0.1, epsilonExp);

    float epsilon = 1.0/2;

    long long int bisectionWidth = INT64_MAX;
    array<set<Number>, 4> bisectionSet;

    while (true) {
        array<set<Number>, 4> candidateBisection = getGoodBisection(graph, epsilon, f);
        if (getCutsize(graph, candidateBisection[1])<bisectionWidth){
            bisectionSet = candidateBisection;
            bisectionWidth = getCutsize(graph, candidateBisection[1]);
        } else
        if (epsilon<=epsilonLimit) break;
        epsilon /= 2;
    }
    for (auto n: bisectionSet[0]){
        if (bisectionSet[1].find(n)==bisectionSet[1].end()) return false;
        if (bisectionSet[3].find(n)!=bisectionSet[3].end()) return false;
        if (bisectionSet[2].find(n)!=bisectionSet[2].end()) return false;
    }
    for (auto n: bisectionSet[2]){
        if (bisectionSet[3].find(n)==bisectionSet[3].end()) return false;
        if (bisectionSet[1].find(n)!=bisectionSet[1].end()) return false;
        if (bisectionSet[0].find(n)!=bisectionSet[0].end()) return false;
    }
    return true;
}

//test if decomposition conditions are fulfilled
bool pathDecompositionTest(Graph &graph, Factory &f, int epsilonExp){
    array<set<Number>, 4> bisectionSet = makeBisection(graph, f, epsilonExp);

    Graph v0Graph(createG(f));
    for (auto &n:bisectionSet[1])
        addV(v0Graph, n, f);
    for (auto &n:bisectionSet[1])
        for (auto &n2: graph[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet[1].find(n2.n2())!=bisectionSet[1].end()))
                addE(v0Graph, Loc(n, n2.n2()), f);
    Graph v1Graph(createG(f));
    for (auto &n:bisectionSet[3])
        addV(v1Graph, n, f);
    for (auto &n:bisectionSet[3])
        for (auto &n2: graph[n])
            if ((n.to_int()<n2.n2().to_int())&&(bisectionSet[3].find(n2.n2())!=bisectionSet[3].end()))
                addE(v1Graph, Loc(n, n2.n2()), f);

    vector<set<Number>> v0Decomposition;
    makeDecomposition(v0Graph, v0Decomposition, bisectionSet[0], bisectionSet[1]);
    vector<set<Number>> v1Decomposition;
    makeDecomposition(v1Graph, v1Decomposition, bisectionSet[2], bisectionSet[3]);

    vector<set<Number>> middleDecomposition;
    middleDecomposition = getMiddleDecomposition(graph, middleDecomposition, bisectionSet[0],
                                                 bisectionSet[2]);

    vector<set<Number>> decomposition;
    decomposition.insert(decomposition.end(), v0Decomposition.rbegin(), v0Decomposition.rend());
    decomposition.insert(decomposition.end(), middleDecomposition.begin(), middleDecomposition.end());
    decomposition.insert(decomposition.end(), v1Decomposition.begin(), v1Decomposition.end());

    //cout<<decomposition<<"\n";

    reduceDecomposition(decomposition);

    return decompositionTest(graph, decomposition);

}

//test if set is at least 1-helpful
bool oneHelpfulSetTest(Graph &graph, set<Number> v0){
    set<Number> s = getHelpfulSet(graph, v0);
    multiset<pair<Number, Number>> cut = getCut(graph, v0);
    return helpfulnessOfSet(graph, v0, s, cut) > 0;
}

//test if returned set fulfills the lemma
bool redBlackEdgesTest(Graph &graph, multiset<pair<Number, Number>> blackEdges){
    set<Number> result = redBlackEdges(graph, blackEdges);
    return isPositive(result, graph, blackEdges);
}

int main(){
    cout << "START OF TEST\n";

    Factory f;
    Graph g1(createG(f)),g2(createG(f)),g3(createG(f)),g4(createG(f)),g5(createG(f)),g6(createG(f)),g7(createG(f)),
    g8(createG(f)), g9(createG(f));
    g1 = open("bigGraph", f);
    g2 = open("graph", f);
    g3 = open("koliskaGraph", f);
    g4 = open("myGraph", f);
    g5 = open("petersen", f);
    g6 = open("snarkJ9", f);
    g7 = open("symetricGraph", f);
    g8 = open("graph100", f);

    assert(pathDecompositionTest(g1, f, 15));
    assert(pathDecompositionTest(g2, f, 15));
    assert(pathDecompositionTest(g3, f, 15));
    assert(pathDecompositionTest(g4, f, 15));
    assert(pathDecompositionTest(g5, f, 15));
    assert(pathDecompositionTest(g6, f, 15));
    assert(pathDecompositionTest(g7, f, 15));
    assert(pathDecompositionTest(g8, f, 15));
    assert(bisectionTest(g1, f, 15));
    assert(bisectionTest(g2, f, 15));
    assert(bisectionTest(g3, f, 15));
    assert(bisectionTest(g4, f, 15));
    assert(bisectionTest(g5, f, 15));
    assert(bisectionTest(g6, f, 15));
    assert(bisectionTest(g7, f, 15));
    assert(bisectionTest(g8, f, 15));

    //ONE-HELPFUL SET
    set<Number> v0;
    for (int i = 0; i<6; i++)
        v0.insert(i);
    assert(oneHelpfulSetTest(g7, v0));
    v0.insert(6);
    assert(oneHelpfulSetTest(g5, v0));
    v0.insert(7);
    assert(oneHelpfulSetTest(g3, v0));
    v0.insert(8);
    assert(oneHelpfulSetTest(g4, v0));
    for (int i = 9; i<18; i++)
        v0.insert(i);
    assert(oneHelpfulSetTest(g1, v0));
    v0.insert(18);
    /*
     * SPECIAL CASE IN NEXT ROW...HERE IS THE CUT TOO SMALL, THAT WE DONT SEARCH FOR 1-HELPFUL SET
     */
    assert(!oneHelpfulSetTest(g2, v0));
    v0.insert(19);
    assert(oneHelpfulSetTest(g6, v0));


    //RED-BLACK EDGES
    multiset<pair<Number, Number>> blackEdges;
    blackEdges.insert(pair(0,1));
    blackEdges.insert(pair(2,3));
    blackEdges.insert(pair(4,5));
    blackEdges.insert(pair(6,7));
    blackEdges.insert(pair(8,9));
    blackEdges.insert(pair(10,11));
    blackEdges.insert(pair(12,13));
    blackEdges.insert(pair(15,16));
    blackEdges.insert(pair(17,18));
    blackEdges.insert(pair(19,20));
    blackEdges.insert(pair(22,23));
    blackEdges.insert(pair(24,25));
    blackEdges.insert(pair(21,26));
    blackEdges.insert(pair(27,29));
    blackEdges.insert(pair(14,30));
    blackEdges.insert(pair(28,31));
    assert(redBlackEdgesTest(g2, blackEdges));

    blackEdges.clear();
    blackEdges.insert(pair(0,1));
    blackEdges.insert(pair(1,2));
    blackEdges.insert(pair(3,4));
    blackEdges.insert(pair(3,13));
    blackEdges.insert(pair(4,13));
    blackEdges.insert(pair(5,14));
    blackEdges.insert(pair(11,12));
    blackEdges.insert(pair(12,13));
    blackEdges.insert(pair(9,10));
    blackEdges.insert(pair(6,7));
    blackEdges.insert(pair(8,15));
    assert(redBlackEdgesTest(g4, blackEdges));

    blackEdges.clear();
    blackEdges.insert(pair(0,11));
    blackEdges.insert(pair(0,12));
    blackEdges.insert(pair(0,13));
    blackEdges.insert(pair(3,12));
    blackEdges.insert(pair(1,7));
    blackEdges.insert(pair(2,10));
    blackEdges.insert(pair(4,5));
    blackEdges.insert(pair(4,6));
    blackEdges.insert(pair(2,8));
    blackEdges.insert(pair(8,9));
    assert(redBlackEdgesTest(g3, blackEdges));

    blackEdges.clear();
    blackEdges.insert(pair(0,2));
    blackEdges.insert(pair(2,4));
    blackEdges.insert(pair(4,9));
    blackEdges.insert(pair(0,7));
    blackEdges.insert(pair(1,9));
    blackEdges.insert(pair(1,6));
    blackEdges.insert(pair(3,6));
    blackEdges.insert(pair(5,8));
    assert(redBlackEdgesTest(g7, blackEdges));

    blackEdges.clear();
    blackEdges.insert(pair(0,1));
    blackEdges.insert(pair(1,2));
    blackEdges.insert(pair(2,18));
    blackEdges.insert(pair(17,29));
    blackEdges.insert(pair(14,15));
    blackEdges.insert(pair(15,16));
    blackEdges.insert(pair(14,28));
    blackEdges.insert(pair(12,13));
    blackEdges.insert(pair(19,27));
    blackEdges.insert(pair(3,20));
    blackEdges.insert(pair(4,21));
    blackEdges.insert(pair(30,31));
    blackEdges.insert(pair(10,11));
    blackEdges.insert(pair(9,26));
    blackEdges.insert(pair(7,8));
    blackEdges.insert(pair(24,25));
    blackEdges.insert(pair(5,22));
    blackEdges.insert(pair(6,23));
    blackEdges.insert(pair(7,24));
    blackEdges.insert(pair(8,24));
    assert(redBlackEdgesTest(g1, blackEdges));
    //TRANSFORMATIONS
    set<Number> cVertices, d1Vertices, d2Vertices, d3Vertices, eVertices;
    cVertices.insert(0);
    cVertices.insert(1);
    cVertices.insert(3);
    d1Vertices.insert(4);
    d1Vertices.insert(6);
    d2Vertices.insert(2);
    eVertices.insert(5);
    map<Number, multiset<Number>> neighbors, transformedNeighbors;
    multiset<Number> n0, n1, n2, n3, n4, n5, n6;
    n0.insert(1);
    n0.insert(6);
    n1.insert(0);
    n1.insert(2);
    n2.insert(1);
    n2.insert(3);
    n2.insert(4);
    n3.insert(2);
    n3.insert(4);
    n4.insert(2);
    n4.insert(3);
    n4.insert(5);
    n5.insert(4);
    n5.insert(6);
    n5.insert(6);
    n6.insert(0);
    n6.insert(5);
    n6.insert(5);
    neighbors[0] = n0;
    neighbors[1] = n1;
    neighbors[2] = n2;
    neighbors[3] = n3;
    neighbors[4] = n4;
    neighbors[5] = n5;
    neighbors[6] = n6;
    vector<pair<pair<pair<Number, Number>, pair<Number, Number>>, pair<pair<Number, Number>, pair<Number, Number>>>>
            executedTransformations;
    transformedNeighbors =
            transformationType1(cVertices, d1Vertices, d2Vertices, d3Vertices, eVertices, neighbors, executedTransformations);
    assert((executedTransformations[0].first.first.first == 0)&&(executedTransformations[0].first.first.second == 2));
    assert((executedTransformations[0].first.second.first == 1)&&(executedTransformations[0].first.second.second == 4));
    assert((executedTransformations[0].second.first.first == 0)&&(executedTransformations[0].second.first.second == 1));
    assert((executedTransformations[0].second.second.first == 2)&&(executedTransformations[0].second.second.second == 4));
    transformationType2(d1Vertices, d2Vertices, d3Vertices, eVertices, transformedNeighbors, executedTransformations);
    assert((executedTransformations[1].first.first.first == 2)&&(executedTransformations[1].first.first.second == 6));
    assert((executedTransformations[1].first.second.first == 0)&&(executedTransformations[1].first.second.second == 5));
    assert((executedTransformations[1].second.first.first == 2)&&(executedTransformations[1].second.first.second == 0));
    assert((executedTransformations[1].second.second.first == 6)&&(executedTransformations[1].second.second.second == 5));

    cout << "END OF TEST";
}

