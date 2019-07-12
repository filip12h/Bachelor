#include <cmath>
#include "oneHelpfulSet.hpp"
#include "io/print_nice.hpp"
//#include "basic_impl.hpp"

inline int getCutSize(Graph &graph, set<Number> v0){
    int cutsize = 0;
    for (auto &n: v0){
        for (auto &neighbor: graph[n])
            if (v0.find(neighbor.n2())==v0.end())
                cutsize++;
    }
    return cutsize;
}

inline int getCutsize(Graph &graph, set<Number> v0, set<Number> v1){
    int cutsize = 0;
    for (auto &n: v0){
        for (auto &neighbor: graph[n])
            if (v1.find(neighbor.n2())!=v1.end())
                cutsize++;
    }
    return cutsize;
}

//we will reach vertices of one of the sets V0 and V1 of the cut pi(V0, V1)
pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>>
        getGoodBisection(Graph &graph, float &epsilon, Factory &f){
    set<Number> v0, v1, allVerticesOfGraph;
    for (int i = 1; i<graph.order(); i++)
        v0.insert(graph[i].n());
    v1.insert(graph[0].n());

    multiset<pair<Number, Number>> cut;
    for (auto &n: v1)
        for (auto &n2: graph[n])
            if (v1.find(n2.n2())==v1.end())
                cut.insert(pair(min(n, n2.n2()), max(n, n2.n2())));

    while (v1.size()<v0.size()){
        //we cant change cut in for auto loop, so we have to mark which edges add and remove from cut.
        multiset<pair<Number, Number>> toErase, toAdd;
        pair<Number, Number> e = cut.begin().operator*();
        if (v1.find(e.first)!=v1.end()){
            v0.erase(e.second);
            v1.insert(e.second);
            for (auto &n: graph[e.second])
                if (v1.find(n.n2())!=v1.end()){
                    toErase.insert(pair(n.n2(), e.second));
                } else {
                    toAdd.insert(pair(n.n2(), e.second));
                }
        } else {
            v0.erase(e.first);
            v1.insert(e.first);
            for (auto &n: graph[e.first])
                if (v1.find(n.n2())!=v1.end()){
                    toErase.insert(pair(n.n2(), e.first));
                } else {
                    toAdd.insert(pair(n.n2(), e.first));
                }
        }
        //update the cut
        for (auto &a: toAdd)
            cut.insert(pair(min(a.first, a.second), max(a.first, a.second)));
        for (auto &r: toErase)
            cut.erase(pair(min(r.first, r.second), max(r.first, r.second)));
    }

    //we move subsets of vertices between v0 and v1, from larger to smaller and we always decrease cutsize under
    // 1/6 + epsilon of vertices in graph

    while (true) {
        if (v1.size() > v0.size()) {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f);
            if (setToMove.empty()) break; //if we havent found setToMove, end while cycle
            for (auto &n: setToMove) {
                v1.erase(n);
                v0.insert(n);
            }
        } else {
            set<Number> setToMove = getHelpfulSet(graph, v0, epsilon, f);
            if (setToMove.empty()) break; //if we havent found setToMove, end while cycle
            for (auto &n: setToMove) {
                v0.erase(n);
                v1.insert(n);
            }
        }
    }

    set<Number> v0Vertices, v1Vertices;
    for (auto &n: v0)
        for (auto &n2: graph[n])
            if (v0.find(n2.n2())==v0.end()) {
                v0Vertices.insert(n);
                v1Vertices.insert(n2.n2());
            }

    return pair(pair(v0Vertices, v0), pair(v1Vertices, v1));
}

pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> makeBisection(Graph &graph, Factory &f){
    cout<<"before start of algorithm, set its precision (and slowness)\n";
    cout<<"type integer from 1 (faster, less precise) to 15 (slower, but precise)\n";
    int epsilonExp;
    cin>>epsilonExp;
    if ((epsilonExp>15)||(epsilonExp<1)){
        cout<<"precision of algorithm is set to 5";
        epsilonExp = 5;
    }
    float epsilonLimit = pow(0.1, epsilonExp);

    float epsilon = 1.0/2;

    //bisectionSet is in ((a, b), (c, d)) format, where b is V0, d is V1 and a(c) are C-vertices in V0(V1)
    long long int bisectionWidth = INT64_MAX;
    pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> bisectionSet;

    while (true) {
        pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>>
                candidateBisection = getGoodBisection(graph, epsilon, f);
        if (getCutsize(graph, candidateBisection.first.second, candidateBisection.second.second)<bisectionWidth){
            bisectionSet = candidateBisection;
            bisectionWidth = getCutsize(graph, candidateBisection.first.second, candidateBisection.second.second);
            //bisectionWidth = max(candidateBisection.first.first.size(), candidateBisection.second.first.size());
        } else {
            //long double limit = (1.0/(pow(graph.order(), 3)));
            //this^ was considered as very low number for some larger graphs
            if (epsilon<=epsilonLimit) //every time I get here, it takes epsilon constant steps to det here
                //in case of smaller graphs, we can increse value of limit, which epsilon has to undergo
                break;
        }
        //to achive possibly better bisection we decrease epsilon
        epsilon /= 2;
    }
    return bisectionSet;
}