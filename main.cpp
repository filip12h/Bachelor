#include <set>
#include <iostream>
#include <basic_impl.hpp>
#include <io/print_nice.hpp>
#include <invariants.hpp>


using namespace ba_graph;
using namespace std;

/*
 * pokus o classu rozsirujucu Graph. Problem s privatnym konstruktorom
 */

//class MyComponent: public Graph{
//public:
//    friend Graph;
//    int numOfDelEdges;
//    bool isLarge;
//    bool isThin;
//    float delta;
//
//
//};

// program kompilujem nasledujucim prikazom
// g++ -std=c++17 -fconcepts main.cpp


    // metoda vrati komponenty grafu

    void DFS(const Rotation &rot, std::map<Vertex, int> &visited, Graph &graph, Factory &f, Graph &g_) {
        // oznac vrchol ako navstiveny a pridaj ho do komponentu
        visited[rot.v()] = true;
        addV(g_, rot.v(), rot.n(), f);

        //kazdy nenavstiveny susedny vrchol pridaj do komponentu aj s hranou ktorou sme ho objavili
        for (auto &inc: rot) {
            Rotation& neighbor = inc.r2();
            if (!visited[neighbor.v()]) {
                addV(g_, neighbor.v(), neighbor.n(), f);
                visited[neighbor.v()] = true;
                DFS(neighbor, visited, graph, f, g_);
            }
        }
    }

    vector<Graph> connectedComponents(Graph &g, Factory &f) {
        vector<Graph> components;
        std::map<Vertex, int> visited;
        for(auto &rot: g) visited[rot.v()]=0;
        Graph graph(createG(f));

        //najdeme nenavstiveny vrchol a nasledne najdeme hrany a vrcholy s nim spojene
        for (auto &rot: g) {
            if (!visited[rot.v()]) {
                DFS(rot, visited, g, f, graph);
                for(auto &rot1: graph)
                    for(auto it: g[rot1.n()].list(IP::primary()))
                        if(graph.contains(it->n2()))
                            addE(graph, rot1, it->r2(), it->e(), f);

                components.push_back(std::move(graph));
            }
        }
        return components;
    }

    /*
     * vyhadzujeme male sety v ktorcyh sa nachadzaju cierne cykly
     */
    void step1(float epsilon, int numOfVertices, vector<Graph> &rodinaKomponentov){
        for (auto &g: rodinaKomponentov){
            if (g.order()<=20/epsilon+41){
                int b1 = 0, b3 = 0;
                //TODO: prejdeme vrcholmi a spocitame pocet vrcholov s 3 ciernymi hranami a s 1 ciernou hranou
                // ak sa rovnaju, tak komponent ma evidentne cyklus
                for (auto &rot1: g){
                    int pocetCiernychHran = 0;
                    for(auto it: g[rot1.n()].list(IP::primary()))
                        if(g.contains(it->n2()))
                            pocetCiernychHran++;
                    assert((pocetCiernychHran>0)&&(pocetCiernychHran<=3));
                    if (pocetCiernychHran==1)
                        b1++;
                    if (pocetCiernychHran==3)
                        b3++;
                }
                if (b1==b3)
                    rodinaKomponentov.erase(remove(rodinaKomponentov.begin(), rodinaKomponentov.end(), g));
            }
        }
    }
    /*
     * vyhadzujeme velke tenke sety aj s malymi setmi cervenou hranou s nimi spojenymi
     */
    void step2(float epsilon, Graph graph, vector<Graph> &rodinaKomponentov){
        //z rodiny komponentov budeme vyhadzovat prvky pokym tam budu, pricom vyhadzoavnim sa mozu zmenit tenke mnoziny
        // na hrube a naopak, preto prehladavanie ukoncime az ked od posledneho najdenia neprejdeme vsetky prvky
        int esteTrebaPrejst = rodinaKomponentov.size();
        for (auto &g: rodinaKomponentov){

        }
        while(true){
            bool vyhodiliSme = 0;
            for (auto &g: rodinaKomponentov){
                //TODO: pokial sme pocas celeho for cyklu nevyhodili nic, prejdeme do riadku kde brejkneme while cyklus.
                // pokial sme vyhodili, tak v tom momente sa vraciame na zaciatok a robime cely for cyklus od zaciatku
                if ("vyhadzujeme"){
                    //vyhodime z rodiny komponentov aj so vsetkymi dalsimi co treba
                    vyhodiliSme = true;
                }
            }
            if (!vyhodiliSme)
                break;
        }

    }

/*
* funkcia z grafu, v ktorom su cervene a cierne hrany take, ze kazdeho vrcholu sa dotyka aspon jedna cierna hrana
* a zaroven nech je cervenych hran viac ako ciernych.
* Najdeme mnozinu urcitej/obmedzenej velkosti taku, ze cervenych interncyh hran je viac ako ciernych externych
*/
void redBlackEdges(int numOfVertices, vector<Edge> redEdges, vector<Edge> blackEdges) {
    assert(redEdges.size()>numOfVertices/2); //podla lemy musi byt pocet cervenych hran vacsi ako pocet vrcholov grafu zmenseny o polovicu
    float epsilon = 0; // nesko mu priradime kladnu hodnotu taku, aby |R|>|V|(1/2+epsilon)

    while ((epsilon<=0)||(epsilon>=1/2)) {
        cout<< "Zvoľ hodnotu epsilon z intervalu (0, 1/2)\n";//pozn: prilis velke alebo male epsilon nam nemusia vyhovovat
        cin >> epsilon;
    }
    Factory f;
    Graph povodnyGraf(createG(f));
    Graph ciernyGraf(createG(f));

    addMultipleV(ciernyGraf, numOfVertices, f);
    addMultipleE(ciernyGraf, blackEdges, f);
    addMultipleV(povodnyGraf, numOfVertices, f);
    addMultipleE(povodnyGraf, blackEdges, f);
    addMultipleE(povodnyGraf, redEdges, f);

    vector<Graph> rodinaKomponentov = connectedComponents(ciernyGraf, f);
    //z rodinaKomponentov budeme neskor vyhadzovat jednotlive elementy, zostane tam velka tenka
    //mnozina s nejakymi vlastnostami a nasledne najdeme mnozinu splnajucu lemu

    float delta = epsilon/(1+2*epsilon);

    //TODO: prejdeme si rodinaKomponentov a male sety



}

int main() {
    Factory f;
    Graph g(createG(f));

    for(int i=0;i<8;i++) addV(g, i, f);
    for(int i=0;i<3;i++) addE(g, Loc(i, i+1), f);
    addE(g, Loc(3,0), f);
    for(int i=4;i<6;i++) addE(g, Loc(i, i+1), f);

    //g[5].list(IP::all(), IT::v2());



    std::cout<<g;
    vector<Graph> sg = connectedComponents(g, f);

    while (!sg.empty())
    {cout<<sg.back();
    sg.pop_back();}

};