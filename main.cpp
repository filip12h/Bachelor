#include <set>
#include <iostream>
#include "../BAgraph/impl/basic/include.hpp"
#include "../BAgraph/io/print_nice.hpp"
#include "../BAgraph/invariants.hpp"


using namespace ba_graph;
using namespace std;

// program kompilujem nasledujucim prikazom
// g++ -std=c++17 -fconcepts main.cpp

int main() {
    Graph g;
    Factory f;

    for(int i=0;i<10;i++) addV(g, i);
    for(int i=0;i<9;i++) addE(g, Location(i, i+1), f);
    addE(g, Location(9,0), f);

    //g[5].find(IP::all(), IT::v2());
    //g[5].find(IT::v2());



    std::cout<<g;


class Lemma1 {
    /*
 * funkcia z grafu, v ktorom su cervene a cierne hrany take, ze kazdeho vrcholu sa dotyka aspon jedna ciarna hrana
 * a zaroven nech je cervenych hran viac ako ciernych.
 * Najdeme mnozinu urcitej/obmedzenej velkosti taku, ze cervenych interncyh hran je viac ako ciernych externych
 */
    void redBlackEdges(set<Vertex> vertices, set<Edge> redEdges, set<Edge> blackEdges) {

    }

    // metoda vrati komponenty grafu
//    set<Graph> connectedComponents(set<Vertex> vertices, set<Edge> edges) {
//        int vs = vertices.size();
//        set<Graph> components;
//        set<Vertex> visited;
//        //najdeme nenavstiveny vrchol a nasledne najdeme hrany a vrcholy s nim spojene
//        for (Vertex v : vertices) {
//            bool is_in = visited.find(v) != visited.end();
//            if (!is_in) {
//
//                Graph graph;
//
//                graph = DFS(vertices, edges, v, visited);
//
//                components.insert(graph);
//            }
//        }
//    }
//
//    Graph DFS(set<Vertex> vertices, set<Edge> edges, Vertex &v, set<Vertex> visited) {
//        // oznac vrchol ako navstiveny
//        visited[v] = true;
//        Graph graph;
//        addV(graph, v);
//
//        //cout << v << " ";
//
//        // prejdeme vsetkymi vrcholmi spojenymi s vrcholom
//        list<int>::iterator i;
//        for ()
//            if (!visited[*i])
//                DFSUtil(*i, visited);
//    }

        // metoda vrati komponenty grafu
public:
//        set<Graph> connectedComponents(int vertices, Graph &g) {
//            set<Graph> components;
//            bool *visited = new bool[vertices];
//
//            //G[n].find(IP::all(),IT::n2())
//
//            for (int i = 0; i < vertices; i++)
//                visited[i] = false;
//            //najdeme nenavstiveny vrchol a nasledne najdeme hrany a vrcholy s nim spojene
//            for (int i = 0; i < vertices; i++) {
//                if (!visited[i]) {
//
//                    Graph graph;
//
//                    graph = DFS(vertices, i, visited, g);
//
//                    components.insert(graph);
//                }
//            }
//            return components;
//        }

        vector<Graph> connectedComponents(int vertices, Graph &g) {
            vector<Graph> components;
            bool *visited = new bool[vertices];

            //G[n].find(IP::all(),IT::n2())

            for (int i = 0; i < vertices; i++)
                visited[i] = false;
            //najdeme nenavstiveny vrchol a nasledne najdeme hrany a vrcholy s nim spojene
            for (int i = 0; i < vertices; i++) {
                if (!visited[i]) {

                    Graph graph;

                    graph = DFS(vertices, i, visited, g);

                    components.push_back(graph);
                }
            }
            return components;
        }

        Graph DFS(int vertices, int v, bool visited[], Graph &graph) {
            // oznac vrchol ako navstiveny a pridaj ho do komponentu
            visited[v] = true;
            Graph g_;
            Factory factory;
            addV(g_, v);

            //kazdy nenavstiveny susedny vrchol pridaj do komponentu aj s hranou ktorou sme ho objavili
            for (int i = 0; i < graph[v].degree(); i++) {
                int neighbor = graph[v][i].n2().n; //funkciou .n  spravime z Number int
                if (!visited[neighbor]) {
                    addV(g_, neighbor, factory);
                    visited[neighbor] = true;
                    DFS(vertices, neighbor, visited, graph);
                }
            }
            return g_;
        }

    };

    Lemma1 l;
    vector<Graph> sg = l.connectedComponents(10, g);

    //cout<<sg;

};