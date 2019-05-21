#include "oneHelpfulSet.cpp"
//#include "basic_impl.hpp"

int getCutsize(Graph &graph, set<Number> v0, set<Number> v1){
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
        for (auto &e: cut){
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
            break;
        }
        //update the cut
        for (auto &a: toAdd)
            cut.insert(pair(min(a.first, a.second), max(a.first, a.second)));
        for (auto &r: toErase)
            cut.erase(pair(min(r.first, r.second), max(r.first, r.second)));
    }
//    for (int i = graph.order()/2; i<graph.order(); i++)
//        v1.insert(graph[i].n());
    //we move subsets of vertices between v0 and v1, from larger to smaller and we always decrease cutsize under
    // 1/6 + epsilon of vertices in graph

    int cutsize = getCutsize(graph, v0, v1);
    int cutsetDifference = graph.order()/2.0; //TODO: this might be variable parameter
    float limit = (1.0/6 + epsilon)*graph.order();
    int v0size = v0.size(), v1size = v1.size();
    while ((abs(v1size-v0size)>=cutsetDifference)||(cutsize>limit)) {
        if (v1.size() > v0.size()) {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f);
            if (setToMove.empty()) break; //if we havent found setToMove, end while cycle
            for (auto &n: setToMove) {
                v1.erase(n);
                v0.insert(n);
            }
        } else {
            set<Number> setToMove = getHelpfulSet(graph, v0, epsilon, f);
            if (setToMove.empty()) break;//if we havent found setToMove, end while cycle
            for (auto &n: setToMove) {
                v0.erase(n);
                v1.insert(n);
            }
        }
        cutsize = getCutsize(graph, v0, v1);
    }

    //check some little moves
//    while (true){
//        if (v1.size() - v0.size() > 1) {
//            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f, true);
//            if (helpfulnessOfSet(graph, v1, setToMove, cut) > 0) {
//                for (auto &n: setToMove) {
//                    v1.erase(n);
//                    v0.insert(n);
//                }
//            } else break;
//        } else {
//            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f, true);
//            if (helpfulnessOfSet(graph, v0, setToMove, cut) > 0) {
//                for (auto &n: setToMove) {
//                    v0.erase(n);
//                    v1.insert(n);
//                }
//            } else break;
//        }
//    }

    //if it is possible to decrease size difference between sections v0 and v1, do it
    while (true) {
        if (v1.size() - v0.size() > 1) {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f);
            if (helpfulnessOfSet(graph, v1, setToMove, cut) > 0) {
                for (auto &n: setToMove) {
                    v1.erase(n);
                    v0.insert(n);
                }
            } else break;
        } else {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f);
            if (helpfulnessOfSet(graph, v0, setToMove, cut) > 0) {
                for (auto &n: setToMove) {
                    v0.erase(n);
                    v1.insert(n);
                }
            } else break;
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