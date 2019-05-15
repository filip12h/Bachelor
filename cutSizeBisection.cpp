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
    for (int i = 0; i<graph.order()/2; i++)
        v0.insert(graph[i].n());
    for (int i = graph.order()/2; i<graph.order(); i++)
        v1.insert(graph[i].n());
    //we move subsets of vertices between v0 and v1, from larger to smaller and we always decrease cutsize under
    // 1/6 + epsilon of vertices in graph

    int cutsize = getCutsize(graph, v0, v1);
    int cutsetDifference = graph.order()/2; //TODO: this might be variable parameter
    float limit = (1.0/6)*graph.order();
    int v0size = v0.size(), v1size = v1.size();
    while ((abs(v1size-v0size)>=cutsetDifference)||(cutsize>limit)) {
        if (v1.size() > v0.size()) {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f, false);
            if (setToMove.empty()) break; //if we havent found setToMove, end while cycle
            for (auto &n: setToMove) {
                v1.erase(n);
                v0.insert(n);
            }
        } else {
            set<Number> setToMove = getHelpfulSet(graph, v0, epsilon, f, false);
            if (setToMove.empty()) break;//if we havent found setToMove, end while cycle
            for (auto &n: setToMove) {
                v0.erase(n);
                v1.insert(n);
            }
        }
        cutsize = getCutsize(graph, v0, v1);
    }

    set<pair<Number, Number>> cut;
    for (auto &n: v0)
        for (auto &n2: graph[n])
            if (v0.find(n2.n2())==v0.end())
                cut.insert(pair(n, n2.n2()));

    //check some little moves
    while (true){
        v1size = v1.size();
        v0size = v0.size(); //for some reason I cannot put functions into if condition
        if (v1size - v0size > 1) {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f, true);
            if (helpfulnessOfSet(graph, v1, setToMove, cut) > 0) {
                for (auto &n: setToMove) {
                    v1.erase(n);
                    v0.insert(n);
                }
            } else break;
        } else {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f, true);
            if (helpfulnessOfSet(graph, v0, setToMove, cut) > 0) {
                for (auto &n: setToMove) {
                    v0.erase(n);
                    v1.insert(n);
                }
            } else break;
        }
    }

    //if it is possible to decrease size difference between sections v0 and v1, do it
    while (true) {
        v1size = v1.size();
        v0size = v0.size(); //for some reason I cannot put functions into if condition
        if (v1size - v0size > 1) {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f, false);
            if (helpfulnessOfSet(graph, v1, setToMove, cut) > 0) {
                for (auto &n: setToMove) {
                    v1.erase(n);
                    v0.insert(n);
                }
            } else break;
        } else {
            set<Number> setToMove = getHelpfulSet(graph, v1, epsilon, f, false);
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