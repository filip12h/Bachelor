//#include "basic_impl.hpp"
#include "redBlackEdges.hpp"
//#include <algorithm>
#include <unordered_set>

using namespace std;
using namespace ba_graph;

inline int helpfulnessOfSet(Graph &graph, set<Number> v0, set<Number> s, multiset<pair<Number, Number>> &cut){
    //there is cut consisting of edges pair<Number, Number> that divide vertices into V0, V1
    //suppose that set S is subset of V0, i is number of vertices connecting S and V1,
    //j is number of vertices connecting S and rest of V0, set Helpfulness is number i-j

    int i = 0, j = 0;

    for (auto &e: cut)
        if ((s.find(e.first)!=s.end())||(s.find(e.second)!=s.end()))
            i++;
    for (auto &v: s)
        for (auto &e: graph[v])
            if ((v0.find(e.n2())!=v0.end())&&(s.find(e.n2())==s.end()))
                j++;
    return i-j;
}

inline multiset<Number> getUnion(const multiset<Number> &a, const multiset<Number> &b)
{
    multiset<Number> result = a;
    result.insert(b.begin(), b.end());
    return result;
}

set<Number> find3ConnectedCVertices(map<Number, multiset<Number>> neighbors, set<Number> cVertices){
    set<Number> result;
    for (auto &v1: cVertices)
        for (auto &v2: neighbors[v1])
            if (cVertices.find(v2)!=cVertices.end())
                for (auto &v3: getUnion(neighbors[v2], neighbors[v1]))
                    if ((v3!=v2)&&(v3!=v1)&&(cVertices.find(v3)!=cVertices.end())){
                        result.insert(v1);
                        result.insert(v2);
                        result.insert(v3);
                        return result;
                    }
    return result; //in this case the result is empty
}

inline set<Number> getCVertices(set<Number> v0, multiset<pair<Number, Number>> &cut){
    //we find all vertices in v0 which are connected to cut edges

    set<Number> vertices;

    for (auto &e: cut)
        if ((v0.find(e.first)!=v0.end())&&(vertices.find(e.first)==vertices.end()))
            vertices.insert(e.first);
        else if ((v0.find(e.second)!=v0.end())&&(vertices.find(e.second)==vertices.end()))
            vertices.insert(e.second);

    return vertices;
}

inline set<Number> getDVertices(map<Number, multiset<Number>> neighbors, set<Number> cVertices, set<Number> v0){
    //we find all vertices connected to cVertices and not in cut

    set<Number> vertices;

    for (auto &v: cVertices)
        for (auto &n: neighbors[v]) //incident to C, contained in V0, definitely it is D vertex
            if ((v0.find(n)!=v0.end())&&(cVertices.find(n)==cVertices.end()))
                vertices.insert(n);
    return vertices;
}

inline set<Number> getEVertices(set<Number> &v0, set<Number> cVertices, set<Number> dVertices){
    //we will find all vertices in v0 not contained in cVertices neither dVertices

    set<Number> vertices;

    for (auto &v: v0)
        if ((cVertices.find(v)==cVertices.end())&&(dVertices.find(v)==dVertices.end()))
            vertices.insert(v);

    return vertices;
}

inline map<Number, multiset<Number>> transformationType1(set<Number> &cVertices, set<Number> &d1Vertices, set<Number> &d2Vertices,
        set<Number> &d3Vertices, set<Number> &eVertices, map<Number, multiset<Number>> neighbors,
        vector<pair<pair<pair<Number, Number>, pair<Number, Number>>, pair<pair<Number, Number>, pair<Number, Number>>>>
                                             &executedTransformations){

    for (auto &v: cVertices) {
        bool doTransformation = false, gamaInD1 = false, deltaInD1 = false;
        //gama belongs to D1 or D2, it is important when updating sets, same with delta in D1 or E
        Number beta, gama, delta;
        for (auto &n: neighbors[v]) {
            if (cVertices.find(n) != cVertices.end())
                for (auto &n2: neighbors[n]) {
                    if ((d1Vertices.find(n2) != d1Vertices.end()) || (d2Vertices.find(n2) != d2Vertices.end())) {
                        if (d1Vertices.find(n2) != d1Vertices.end()) gamaInD1 = true;
                        for (auto &n3: neighbors[n2]) {
                            if ((d1Vertices.find(n3) != d1Vertices.end()) ||
                                (eVertices.find(n3) != eVertices.end())) {
                                //alfa = v;
                                if (d1Vertices.find(n3) != d1Vertices.end()) deltaInD1 = true;
                                beta = n;
                                gama = n2;
                                delta = n3;
                                doTransformation = true;
                            }
                            if (doTransformation) break;
                        }
                    }
                    if (doTransformation) break;
                }
            if (doTransformation) break;
        }
        if (doTransformation) {
            neighbors[v].erase(beta);
            neighbors[beta].erase(v);
            neighbors[gama].erase(delta);
            neighbors[delta].erase(gama);
            neighbors[v].insert(gama);
            neighbors[gama].insert(v);
            neighbors[beta].insert(delta);
            neighbors[delta].insert(beta);
            executedTransformations.push_back(pair(pair(pair(v, gama), pair(beta, delta)), pair(pair(v, beta),
                                                                                             pair(gama, delta))));
            //update d1, d2, d3, e sets
            if (gamaInD1) {
                d1Vertices.erase(gama);
                d2Vertices.insert(gama);
            } else {
                d2Vertices.erase(gama);
                d3Vertices.insert(gama);
            }
            if (deltaInD1) {
                d1Vertices.erase(delta);
                d2Vertices.insert(delta);
            } else {
                eVertices.erase(delta);
                d1Vertices.insert(delta);
            }
        }
    }
    //in this moment there shouldnt be any adjecent C vertices, but who knows...

    return neighbors;

}

inline map<Number, multiset<Number>> transformationType2(set<Number> &d1Vertices, set<Number> &d2Vertices,
        set<Number> &d3Vertices, set<Number> &eVertices, map<Number, multiset<Number>> neighbors,
        vector<pair<pair<pair<Number, Number>, pair<Number, Number>>, pair<pair<Number, Number>, pair<Number, Number>>>>
                                             &executedTransformations){

    bool deltaInD1, doTransformation;
    Number beta, gama, delta;
    set<Number> copyOfD3Vertices;
    for (auto &n: d3Vertices)
        copyOfD3Vertices.insert(n);
    for (auto &v: d3Vertices){
        deltaInD1 = false; doTransformation = false;
        //we dont need to check if neighbor is already in previous neighbors, because "if" denies repeating of vertices
        for (auto &n: neighbors[v]){
            for (auto &n2: neighbors[n]){
                if ((d1Vertices.find(n2) != d1Vertices.end()) || (d2Vertices.find(n2) != d2Vertices.end()))
                    for (auto &n3: neighbors[n2]) {
                        if ((d1Vertices.find(n3) != d1Vertices.end()) ||
                            (eVertices.find(n3) != eVertices.end())) {
                            //alfa = v;
                            if (d1Vertices.find(n3) != d1Vertices.end()) deltaInD1 = true;
                            beta = n;
                            gama = n2;
                            delta = n3;
                            doTransformation = true;
                        }
                        if (doTransformation) break;
                    }
                if (doTransformation) break;
            }
            if (doTransformation) break;
        }
        if (doTransformation) {
            neighbors[v].erase(beta);
            neighbors[beta].erase(v);
            neighbors[gama].erase(delta);
            neighbors[delta].erase(gama);
            neighbors[v].insert(gama);
            neighbors[gama].insert(v);
            neighbors[beta].insert(delta);
            neighbors[delta].insert(beta);
            executedTransformations.push_back(pair(pair(pair(v,gama), pair(beta,delta)), pair(pair(v,beta),
                    pair(gama,delta))));
            //update d1, d2, d3, e sets
            copyOfD3Vertices.erase(v); //I cannot erase elements of d3Vertices in for loop of d3Vertices
            d2Vertices.insert(v);
            //we dont need to check where is gamma, because it will stay in D1 or D2
            if (deltaInD1){
                d1Vertices.erase(delta);
                d2Vertices.insert(delta);
            } else {
                eVertices.erase(delta);
                d1Vertices.insert(delta);
            }
        }
    }
    d3Vertices.clear();
    for (auto &n: copyOfD3Vertices)
        d3Vertices.insert(n); //because we use reference of d3Vertices in call of function

    return neighbors;
}

inline multiset<pair<Number, Number>> getCut(Graph &graph, set<Number> v0){
    multiset<pair<Number, Number>> cut;
    for (auto &v: v0)
        for (auto &e: graph[v])
            if (v0.find(e.n2())==v0.end())
                cut.insert(pair(v, e.n2()));
    return cut;
}

//lemma 2, see more in pdf file
//simpleSteps is true when we check only directly catchable cases
set<Number> getHelpfulSet(Graph &graph, set<Number> v0, float epsilon, Factory &f){

    multiset<pair<Number, Number>> cut = getCut(graph, v0);

    set<Number> vertices;

    //cutsize has to be larger to continue...
    if (cut.size() <= (1.0/3 + 2*epsilon)*v0.size())
        return vertices;

    map<Number, multiset<Number>> neighbors, transformedNeighbors;

    for (auto &rot: graph)
        for (auto &inc: graph[rot.n()])
            neighbors[rot.n()].insert(inc.n2());

    set<Number> cVertices = getCVertices(v0, cut);

    //at first we will check if any C vertex has 2 vertices connecting it to V1
    for(auto &v: cVertices) {
        int numOfNeighborsInside = 0;
        for (auto &n:neighbors[v])
            if (v0.find(n) != v0.end())
                numOfNeighborsInside++;
        if (numOfNeighborsInside<=1){
            vertices.insert(v);
            return vertices;
        }
    }

    //second step: check if there are 3 connected C vertices, which are the set fulfilling lemma
    set<Number> possibleResult = find3ConnectedCVertices(neighbors, cVertices);
    if (possibleResult.size()==3) return possibleResult;


    set<Number> dVertices = getDVertices(neighbors, cVertices, v0);
    set<Number> eVertices = getEVertices(v0, cVertices, dVertices);

    set<Number> d1Vertices, d2Vertices, d3Vertices; //dnVertices are vertices connected to n C vertices

    for (auto &v: dVertices) {
        int numOfNeighbors = 0;
        for (auto &e: graph[v])
            if (cVertices.find(e.n2())!=cVertices.end())
                numOfNeighbors++;
        if (numOfNeighbors==1)
            d1Vertices.insert(v);
        else if (numOfNeighbors==2)
            d2Vertices.insert(v);
        else d3Vertices.insert(v); //possible control if numOfNeighbors==3;
    }

    //at the end we check last general structure we can find directly (case (v) in proof of lemma)
    for (auto &v: cVertices){
        vector<Number> neighborsOfCVertex;
        for (auto &n: neighbors[v])
            if (v0.find(n)!=v0.end())
                neighborsOfCVertex.push_back(n);
        if (neighborsOfCVertex.size()==2){ //always true, because C vertex with 2 cut edges are already checked out
            bool structureIsGood = false;
            if ((cVertices.find(neighborsOfCVertex[0])!=cVertices.end())||
                (d3Vertices.find(neighborsOfCVertex[0])!=d3Vertices.end())) {
                structureIsGood = true;
                for (auto &n: neighbors[neighborsOfCVertex[1]])
                    if ((cVertices.find(n) == cVertices.end()) && (d2Vertices.find(n) == d2Vertices.end()) &&
                        (d3Vertices.find(n) == d3Vertices.end()))
                        structureIsGood = false;
            }
            if (structureIsGood){
                set<Number> goodStructure;
                goodStructure.insert(neighborsOfCVertex[1]);
                for (auto &n1: neighbors[neighborsOfCVertex[1]]) {
                    goodStructure.insert(n1);
                    for (auto &n2: neighbors[n1])
                        if (cVertices.find(n2)!=cVertices.end())
                            goodStructure.insert(n2);
                }
                for (auto &n2: neighbors[neighborsOfCVertex[0]])
                    if (cVertices.find(n2)!=cVertices.end())
                        goodStructure.insert(n2);
                set<Number> result(goodStructure.begin(), goodStructure.end());
                return result;
            }
            if ((cVertices.find(neighborsOfCVertex[1])!=cVertices.end())||
                (d3Vertices.find(neighborsOfCVertex[1])!=d3Vertices.end())) {
                structureIsGood = true;
                for (auto &n: neighbors[neighborsOfCVertex[0]])
                    if ((cVertices.find(n) == cVertices.end()) && (d2Vertices.find(n) == d2Vertices.end()) &&
                        (d3Vertices.find(n) == d3Vertices.end()))
                        structureIsGood = false;
            }
            if (structureIsGood){
                set<Number> goodStructure;
                goodStructure.insert(neighborsOfCVertex[0]);
                for (auto &n1: neighbors[neighborsOfCVertex[0]]) {
                    goodStructure.insert(n1);
                    for (auto &n2: neighbors[n1])
                        if (cVertices.find(n2)!=cVertices.end())
                            goodStructure.insert(n2);
                }
                for (auto &n2: neighbors[neighborsOfCVertex[1]])
                    if (cVertices.find(n2)!=cVertices.end())
                        goodStructure.insert(n2);
                set<Number> result(goodStructure.begin(), goodStructure.end());
                return result;
            }

        }

    }

    //every executed transformation includes 2 added vertices and 2 deleted vertices
    //at the first position there is couple of added edges, at the second position couple of deleted edges
    vector<pair<pair<pair<Number, Number>, pair<Number, Number>>, pair<pair<Number, Number>, pair<Number, Number>>>>
            executedTransformations1;
    vector<pair<pair<pair<Number, Number>, pair<Number, Number>>, pair<pair<Number, Number>, pair<Number, Number>>>>
            executedTransformations2;

    transformedNeighbors = transformationType1(cVertices, d1Vertices, d2Vertices, d3Vertices, eVertices, neighbors,
            executedTransformations1);
    transformedNeighbors = transformationType2(d1Vertices, d2Vertices, d3Vertices, eVertices,
            transformedNeighbors, executedTransformations2);

    Graph redBlackGraph(createG());
    Graph onlyBlackGraph(createG());
    for (auto &v: d1Vertices) {
        addV(redBlackGraph, v);
        addV(onlyBlackGraph, v);
    }
    for (auto &v: d2Vertices){
        addV(redBlackGraph, v);
        addV(onlyBlackGraph, v);
    }
    for (auto &v: d3Vertices){
        addV(redBlackGraph, v);
        addV(onlyBlackGraph, v);
    }
    for (auto &v: eVertices){
        addV(redBlackGraph, v);
        addV(onlyBlackGraph, v);
    }

    multiset<pair<Number, Number>> blackEdges;
    multiset<pair<Number, Number>> redEdges;

    //we add edges between D and E vertices to blackEdges, others are red
    for (auto &v: transformedNeighbors)
        if (((eVertices.find(v.first)!=eVertices.end())||(d1Vertices.find(v.first)!=d1Vertices.end()))||
                ((d2Vertices.find(v.first)!=d2Vertices.end())||(d3Vertices.find(v.first)!=d3Vertices.end()))) {
            for (auto &n: transformedNeighbors[v.first])
                if ((((eVertices.find(n) != eVertices.end()) || (d1Vertices.find(n) != d1Vertices.end())) ||
                    ((d2Vertices.find(n) != d2Vertices.end()) || (d3Vertices.find(n) != d3Vertices.end())))&&(
                            n.to_int()>v.first.to_int())) //every edge only once
                    blackEdges.insert(pair(v.first, n));
        } else if (cVertices.find(v.first)!=cVertices.end()){
            vector<Number> dNeighbors;
            //vertices which are connected to common C vertex, add edge between them to rededges;
            for (auto &n: transformedNeighbors[v.first])
                //there are no d3 vertices
                if ((d1Vertices.find(n)!=d1Vertices.end())||(d2Vertices.find(n)!=d2Vertices.end()))
                    dNeighbors.push_back(n);
            if (dNeighbors.size()==2) {
                redEdges.insert(pair(dNeighbors[0], dNeighbors[1])); //after transformation there should be 2 D vertices
            }
        }

    for (auto &e: blackEdges) {
        addE(redBlackGraph, Loc(e.first, e.second));
        addE(onlyBlackGraph, Loc(e.first, e.second));
    }
    for (auto &e: redEdges)
        addE(redBlackGraph, Loc(e.first, e.second));

    set<Number> derivedSetS = redBlackEdges(redBlackGraph, blackEdges, epsilon*3, onlyBlackGraph, f);

    //for (auto &trans: executedTransformations){
    for (auto trans = executedTransformations2.rbegin(); trans!=executedTransformations2.rend(); trans++)
        if (((derivedSetS.find(trans->first.first.first)!=derivedSetS.end())&&
        (derivedSetS.find(trans->first.first.second)!=derivedSetS.end()))&&(
                (derivedSetS.find(trans->first.second.first)==derivedSetS.end())&&
                (derivedSetS.find(trans->first.second.second)==derivedSetS.end())))
            derivedSetS.insert(trans->first.second.first);
        else if (((derivedSetS.find(trans->first.first.first)==derivedSetS.end())&&
                  (derivedSetS.find(trans->first.first.second)==derivedSetS.end()))&&(
                         (derivedSetS.find(trans->first.second.first)!=derivedSetS.end())&&
                         (derivedSetS.find(trans->first.second.second)!=derivedSetS.end()))){
            derivedSetS.insert(trans->first.first.first);
            derivedSetS.insert(trans->first.first.second);
            for (auto &n: transformedNeighbors[trans->first.first.first])
                if ((cVertices.find(n)!=cVertices.end())&&(derivedSetS.find(n)==derivedSetS.end()))
                    derivedSetS.insert(n);
        }
    for (auto trans = executedTransformations1.rbegin(); trans!=executedTransformations1.rend(); trans++)
        if (((derivedSetS.find(trans->first.first.first)!=derivedSetS.end())&&
             (derivedSetS.find(trans->first.first.second)!=derivedSetS.end()))&&(
                    (derivedSetS.find(trans->first.second.first)==derivedSetS.end())&&
                    (derivedSetS.find(trans->first.second.second)==derivedSetS.end())))
            derivedSetS.insert(trans->first.second.first);
        else if (((derivedSetS.find(trans->first.first.first)==derivedSetS.end())&&
                  (derivedSetS.find(trans->first.first.second)==derivedSetS.end()))&&(
                         (derivedSetS.find(trans->first.second.first)!=derivedSetS.end())&&
                         (derivedSetS.find(trans->first.second.second)!=derivedSetS.end()))){
            derivedSetS.insert(trans->first.first.first);
            derivedSetS.insert(trans->first.first.second);
        }

    for (auto &v: derivedSetS) {
        vertices.insert(v);
        for (auto &n: neighbors[v])
            if ((derivedSetS.find(n)==derivedSetS.end())&&(cVertices.find(n)!=cVertices.end()))
                vertices.insert(n);
    }


    if (v0.size()-vertices.size() + 5 >= 2 * graph.order() / 5.0) {
        return vertices;
    } else {
        vertices.clear();
        return vertices;
    }
}

set<Number> getHelpfulSet(Graph &graph, set<Number> v0, float epsilon){
    Factory f;
    if ((1.0/3+2*epsilon)*v0.size()<getCut(graph, v0).size())
        cout<<getHelpfulSet(graph, v0, epsilon, f);
    else cerr<<"Bad value of epsilon.\n";
    return getHelpfulSet(graph, v0, epsilon, f);
}

set<Number> getHelpfulSet(Graph &graph, set<Number> v0){
    Factory f;
    //epsilon must be lower than (getCut(graph, v0).size()/v0.size()-1/3)/2, so therefore it is 2.01
    float epsilon = ((getCut(graph, v0).size()/float(v0.size()))-1.0/3)/2.01;
    return getHelpfulSet(graph, v0, epsilon, f);
}