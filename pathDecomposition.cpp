#include <iostream>
#include "basic_impl.hpp"

using namespace std;
using namespace ba_graph;

void DFS(Number n, map<Number, int> &visited, Graph &g, set<Number> &vertices, set<Number> &possibleResult) {
    // oznac vrchol ako navstiveny a pridaj ho do komponentu
    visited[n] = true;
    possibleResult.insert(n);

    //kazdy nenavstiveny susedny vrchol pridaj do komponentu aj s hranou ktorou sme ho objavili
    for (auto &inc: g[n]){
        Number neighbor = inc.n2();
        if ((vertices.find(inc.n2())!=vertices.end())&&(!visited[neighbor])){
            possibleResult.insert(neighbor);
            visited[neighbor] = true;
            DFS(neighbor, visited, g, vertices, possibleResult);
        }
    }
}

set<Number> getTreeComponent(Graph &graph, set<Number> vertices){
    map<Number, int> visited;
    for(auto &rot: graph) visited[rot.n()]=0;

    //najdeme nenavstiveny vrchol a nasledne najdeme hrany a vrcholy s nim spojene
    for (auto &rot: graph) {
        if ((vertices.find(rot.n())!=vertices.end())&&(!visited[rot.n()])) { //only vertices in set "vertices"
            set<Number> possibleResult;
            DFS(rot.n(), visited, graph, vertices, possibleResult);
            int numOfEdges = 0;
            for (auto &n : possibleResult)
                for (auto &inc : graph[n])
                    if ((possibleResult.find(inc.n2())!=possibleResult.end())&&(rot.n()>inc.n2()))
                        numOfEdges++;
            if (possibleResult.size()>numOfEdges)
                return possibleResult;
        }
    }
    //return NULL;
}

//this is not actualy tree decomposition, but path decomposition of a tree
//we have to know from what vertex did we come from. that is the purpose of "from"
vector<set<Number>> treeDecomposition(Graph &graph, set<Number> treeVertices, Number vertex, Number from){
    vector<set<Number>> decomposition;
    int numOfNeighbors = 0;
    for (auto &inc: graph[vertex]){
        if ((treeVertices.find(inc.n2())!=treeVertices.end())&&(from.to_int()!=inc.n2().to_int())){
            numOfNeighbors++;
            decomposition = treeDecomposition(graph, treeVertices, inc.n2(), vertex);
        }
    }
    if (numOfNeighbors==0){
        set<Number> bag;
        bag.insert(vertex);
        decomposition.push_back(bag);
    }
    for (auto &bag: decomposition)
        bag.insert(vertex);
    return decomposition;
}

//TODO: function will reduce components, i.e. bags which are subsets of neighbor bags will be removed
// we will get value of pathwidth
int reduceDecomposition(vector<set<Number>> &decomposition){


    return 0;
}

vector<set<Number>> getMiddleDecomposition(Graph &graph, vector<set<Number>> &decomposition, set<Number> &v0Vertices,
        set<Number> v1Vertices){
    set<Number> newBag;
    for (auto &n: v0Vertices)
        newBag.insert(n);
    decomposition.push_back(newBag);

    bool endOfDecomp = true;
    for (auto &n: v0Vertices)
        if (v1Vertices.find(n)==v1Vertices.end())
            endOfDecomp = false;
    if (endOfDecomp){
        return decomposition;
    } else {
        Number vertex;
        for (auto &n: v0Vertices)
            if (v1Vertices.find(n)==v1Vertices.end()){
                vertex = n;
                break;
            }
        for (auto &inc: graph[vertex]){
            if (v1Vertices.find(inc.n2())!=v1Vertices.end())
                v0Vertices.insert(inc.n2());
        }
        decomposition.push_back(v0Vertices);
        v0Vertices.erase(vertex);
        decomposition = getMiddleDecomposition(graph, decomposition, v0Vertices, v1Vertices);
    }
    return decomposition;
}

//until 25th set of decomposition of bigGraph working well
vector<set<Number>> makeDecomposition(Graph &graph, vector<set<Number>> &decomposition, set<Number> &currentSet,
        set<Number> &verticesToDecompose){
    int setSize = currentSet.size();
    if (setSize>0) {
        decomposition.push_back(currentSet);
        bool case3possible = true; // case 3 is possible if every vertex in currentSet has at least 2 neighbors outside
        for (auto &n: currentSet) {
            int neighborsOutside = 0;
            for (auto &n2: graph[n])//we count number of neighbors of n outside currentSet
                if ((currentSet.find(n2.n2()) == currentSet.end()) &&
                (verticesToDecompose.find(n2.n2()) != verticesToDecompose.end()))
                    neighborsOutside++;
            if (neighborsOutside == 0) { // case 1
                case3possible = false;
                verticesToDecompose.erase(n);
                set<Number> newSet;
                for (auto &v: currentSet)
                    newSet.insert(v);
                newSet.erase(n);
                decomposition = makeDecomposition(graph, decomposition, newSet, verticesToDecompose);
                return decomposition;
            } else if (neighborsOutside == 1){ // case 2
                case3possible = false;
                Number neighbor;
                for (auto &n2: graph[n])//we count number of neighbors of n outside currentSet
                    if ((currentSet.find(n2.n2()) == currentSet.end()) &&
                        (verticesToDecompose.find(n2.n2()) != verticesToDecompose.end()))
                        neighbor = n2.n2();
                set<Number> newSet;
                for (auto &v: currentSet)
                    newSet.insert(v);
                newSet.insert(neighbor);
                decomposition.push_back(newSet);
                newSet.erase(n);
                verticesToDecompose.erase(n);
                decomposition = makeDecomposition(graph, decomposition, newSet, verticesToDecompose);
                return decomposition;
            }
        }
        if (case3possible){ // case 3
            int graphSize = graph.order();
            if (setSize >= graphSize / 3 + 1){ // case 3.A ... graphSize/3 rounded down
                set<Number> vertices;
                for (auto &n: verticesToDecompose)
                    if (currentSet.find(n)==currentSet.end())
                        vertices.insert(n);
                set<Number> treeComponent = getTreeComponent(graph, vertices);
                vector<set<Number>> treeBags = treeDecomposition(graph, treeComponent, treeComponent.begin().operator*(),
                        currentSet.begin().operator*());
                for (auto &bag: treeBags) {
                    for (auto &n: currentSet)
                        bag.insert(n);
                    decomposition.push_back(bag);
                }
                //cout<<"WARNING: is this working properly???";
            } else { // case 3.B ... setSize < graphSize/3 +1
                set<Number> verticesToRemove;
                for (auto &n: verticesToDecompose)
                    if (currentSet.find(n)==currentSet.end()) {
                        currentSet.insert(n);
                        verticesToRemove.insert(n);
                        setSize = currentSet.size();
                        if (setSize >= graphSize / 3 + 1)
                            break;
                    }
//                for (auto &n: verticesToRemove)
//                    verticesToDecompose.erase(n);
                decomposition = makeDecomposition(graph, decomposition, currentSet, verticesToDecompose);
                    return decomposition;
            }
        }
        return decomposition;
    } else //if (setSize==1){
        return decomposition;
}