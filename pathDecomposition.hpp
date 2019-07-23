#include <iostream>
#include <cmath>
#include "basic_impl.hpp"
#include "cutSizeBisection.hpp"
#include "io/print_nice.hpp"

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
            unsigned int numOfEdges = 0;
            for (auto &n : possibleResult)
                for (auto &inc : graph[n])
                    if ((possibleResult.find(inc.n2())!=possibleResult.end())&&(n>=inc.n2()))
                        numOfEdges++;
            if (possibleResult.size()>numOfEdges)
                return possibleResult;
        }
    }
    return vertices;
}

//this is not actualy tree decomposition, but path decomposition of a tree
//we have to know from what vertex did we come from. that is the purpose of "from"
vector<set<Number>> treeDecomposition(Graph &graph, set<Number> treeVertices, Number vertex, Number parent){
    vector<set<Number>> decomposition, subdecomposition;
    int numOfNeighbors = 0;
    for (auto &inc: graph[vertex]){
        //if neighbor is from treeVertices and is not a parent
        if ((treeVertices.find(inc.n2())!=treeVertices.end())&&(parent.to_int()!=inc.n2().to_int())){
            numOfNeighbors++;
            subdecomposition = treeDecomposition(graph, treeVertices, inc.n2(), vertex);
            for (auto &bag: subdecomposition)
                decomposition.push_back(bag);
        }
    }
    if (numOfNeighbors==0){
        set<Number> bag;
        decomposition.push_back(bag);
    }
    for (auto &bag: decomposition)
        bag.insert(vertex);
    return decomposition;
}

//function will reduce components, i.e. bags which are subsets of neighbor bags will be removed
// we will get value of decomposition's width
inline int reduceDecomposition(vector<set<Number>> &decomposition){
    int decSize = decomposition.size();
    unsigned int pathWidth = 0;
    for (int i = 0; i<decSize-1; i++) {
        if (decomposition[i].size()>pathWidth)
            pathWidth = decomposition[i].size();
        if (includes(decomposition[i].begin(),decomposition[i].end(),decomposition[i+1].begin(),decomposition[i+1].end())){
            decomposition.erase(decomposition.begin()+i+1);
            decSize--;
            i--; //after removing element we want to compare it again with new neighbors
        } else if (includes(decomposition[i+1].begin(),decomposition[i+1].end(),decomposition[i].begin(),decomposition[i].end())){
            decomposition.erase(decomposition.begin()+i);
            decSize--;
            i--;
        }
    }

    return pathWidth-1;
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

vector<set<Number>> makeDecomposition(Graph &graph, vector<set<Number>> &decomposition, set<Number> &currentSet,
        set<Number> &verticesToDecompose){
    int setSize = currentSet.size();
    if (setSize>0) {
        decomposition.push_back(currentSet);
        for (auto &n: currentSet) {
            int neighborsOutside = 0;
            for (auto &n2: graph[n])//we count number of neighbors of n outside currentSet
                if ((currentSet.find(n2.n2()) == currentSet.end()) &&
                (verticesToDecompose.find(n2.n2()) != verticesToDecompose.end()))
                    neighborsOutside++;
            if (neighborsOutside == 0) { // case 1
                verticesToDecompose.erase(n);
                set<Number> newSet;
                for (auto &v: currentSet)
                    newSet.insert(v);
                newSet.erase(n);
                decomposition = makeDecomposition(graph, decomposition, newSet, verticesToDecompose);
                return decomposition;
            } else if (neighborsOutside == 1){ // case 2
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
        // case 3
        int graphSize = verticesToDecompose.size();
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
            for (auto &n: treeComponent)
                verticesToDecompose.erase(n);
            decomposition = makeDecomposition(graph, decomposition, currentSet, verticesToDecompose);
            return decomposition;
        } else { // case 3.B ... setSize < graphSize/3 +1
            bool addedVertex = false;
            for (auto &n: currentSet) {
                for (auto &inc: graph[n]) {
                    if ((currentSet.find(inc.n2()) == currentSet.end())&&
                        (verticesToDecompose.find(inc.n2())!=verticesToDecompose.end())) {
                        currentSet.insert(inc.n2());
                        addedVertex = true;
                        break;
                    }
                }
                if (addedVertex) break;
            }
            decomposition = makeDecomposition(graph, decomposition, currentSet, verticesToDecompose);
                return decomposition;
        }
        //return decomposition;
    } else //if (setSize==1){
        return decomposition;
}

inline bool decompositionTest(Graph &graph_calculate, vector<set<Number>> decomposition){
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
                if ((bag.find(neighbor.n2())!=bag.end())&&(!checked[neighbor.e().to_int()])){
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
    return goodDecomposition;
}

vector<set<Number>> pathDecomposition(Graph &graph, int epsilonExp){

    Factory f;
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
    middleDecomposition = getMiddleDecomposition(graph, middleDecomposition, bisectionSet[0], bisectionSet[2]);

    vector<set<Number>> decomposition;
    decomposition.insert(decomposition.end(), v0Decomposition.rbegin(), v0Decomposition.rend());
    decomposition.insert(decomposition.end(), middleDecomposition.begin(), middleDecomposition.end());
    decomposition.insert(decomposition.end(), v1Decomposition.begin(), v1Decomposition.end());

    int sizeOfDecomposition = reduceDecomposition(decomposition);

    cout<<"Size of the path decomposition is: "<<sizeOfDecomposition<<"\n\n";

    return decomposition;
}

vector<set<Number>> pathDecomposition(Graph &graph){
    return pathDecomposition(graph, 5);
}