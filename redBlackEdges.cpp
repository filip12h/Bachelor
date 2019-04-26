#include <vector>
#include <basic_impl.hpp>
#include <assert.h>
#include <algorithm>


using namespace ba_graph;
using namespace std;

//tuto budem riesit lemmu 1

void DFS(const Rotation &rot, map<Vertex, int> &visited, Graph &graph, Factory &f, Graph &g_) {
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

// metoda vrati komponenty grafu
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
* funkcia z grafu, v ktorom su cervene a cierne hrany take, ze kazdeho vrcholu sa dotyka aspon jedna cierna hrana
* a zaroven nech je cervenych hran viac ako ciernych.
* Najdeme mnozinu urcitej/obmedzenej velkosti taku, ze cervenych interncyh hran je viac ako ciernych externych
*/
vector<Vertex> redBlackEdges(Graph g, set<Edge> blackEdges, float epsilon) {
    assert(blackEdges.size()<=g.size()/2); //podla lemy musi byt pocet cervenych hran vacsi ako pocet vrcholov grafu zmenseny o polovicu

    Factory f;
    Graph ciernyGraf(createG(f));
    for(auto &e: blackEdges)
        addE(ciernyGraf, e, f);

    vector<Graph> rodinaKomponentov = connectedComponents(ciernyGraf, f);
    //z rodinaKomponentov budeme neskor vyhadzovat jednotlive elementy, zostane tam velka tenka
    //mnozina s nejakymi vlastnostami a nasledne najdeme mnozinu splnajucu lemu

    float delta = epsilon/(1+2*epsilon);

}

//if(std::find(v.begin(), v.end(), x) != v.end()) {
//    /* v contains x */
//}
//



// set is positive if it has more internal red edges, negative if there are more external black edges, neutral if equal
int isPositive(std::set<Vertex> vertices, Graph g, std::set<Edge> blackEdges){
    int intRedEdges = 0, extBlackEdges = 0;
    for (auto &rot: g){
        if (vertices.find(rot.v())!=vertices.end()) //overujeme ci dany vrchol rot sa nachadza v nasej mnozine, kedze iba ta nas zaujima
            for (auto it: g[rot.n()].list(IP::primary())){
                //pokial je sused mimo nasej mnoziny a je spojeny ciernou hranou, ta musi byt externa
                if ((blackEdges.find(it->e())!=blackEdges.end())&&(vertices.find(it->v2()) == vertices.end()))
                    extBlackEdges++;
                else if ((blackEdges.find(it->e())==blackEdges.end())&&(vertices.find(it->v2()) != vertices.end()))
                    intRedEdges++;
            }
    }
    if (intRedEdges>extBlackEdges)
        return 1;
    else if (intRedEdges<extBlackEdges)
        return -1;
    else return 0;
}

//trosku zbytocna funkcia, ale budis...
//pri step1 vyzadujem mnozinu
bool isLarge(std::set<Vertex> &vertices, Graph &g, std::set<Edge> blackEdges, float epsilon){
    return vertices.size() > (1+2*epsilon)/epsilon;
}

//optional TODO: opakujuci sa kod vo funkciach alfa() a s()

//pocet cervenych hran medzi mnozinou vertices a malymi mnozinami
int alfa(std::set<Vertex> vertices, Graph &g, set<Edge> blackEdges, float epsilon, vector<Graph> connectedComponents){
    int result = 0;
    for (auto &rot: g){
        if (vertices.find(rot.v())!=vertices.end()) //pokial dany vrchol rot je v nasej mnozine vertices
            for (auto it: g[rot.n()].list(IP::primary())){ //prejdeme incidentne hrany vrchola
                //pokial je hrana cervena a vrchol na druhej strane je mimo mnoziny
                if ((blackEdges.find(it->e())==blackEdges.end())&&(vertices.find(it->v2()) != vertices.end()))
                    for (auto &comp: connectedComponents){ //prejdeme postupne komponenty
                        //najdeme komponent v ktorom je v2, komponent musi byt maly
                        if ((comp.contains(it->v2()))&&(!isLarge(vertices, g, blackEdges, epsilon)))
                            result++;
                    }
            }
    }
    return result;
}

//velkost zjednotenia mnoziny vertices s malymi mnozinami spojenymi cervenou hranou
int s(std::set<Vertex> vertices, Graph &g, set<Edge> blackEdges, float epsilon, vector<Graph> connectedComponents){
    int result = vertices.size();
    for (auto &rot: g){
        if (vertices.find(rot.v())!=vertices.end())
            for (auto it: g[rot.n()].list(IP::primary())){
                if ((blackEdges.find(it->e())==blackEdges.end())&&(vertices.find(it->v2()) != vertices.end()))
                    for (auto &comp: connectedComponents){
                        if ((comp.contains(it->v2()))&&(!isLarge(vertices, g, blackEdges, epsilon))) {
                            result = +comp.size();
                            connectedComponents.erase(find(connectedComponents.begin(), connectedComponents.end(), comp));
                        }
                    }
            }
    }

    return result;


}

bool isThin(std::set<Vertex> vertices, Graph g, std::set<Edge> blackEdges, float epsilon, vector<Graph> connectedComponents){
    float delta = epsilon/(1+2*epsilon);
    return s(vertices, g, blackEdges, epsilon, connectedComponents)<=
            (alfa(vertices, g, blackEdges, epsilon, connectedComponents)+1)/delta;

}

//strom ma n-1 hran, tie zvysne su vyhoditelne
//number of edges which can be removed without splitting it into 2 or more components
//kolko cyklov tam je, tolko hran vieme vyhodit
/*
 * pre spravne fungovanie musi byt mnozina vertices niektory jeden cely cierny komponent
 */
int r(std::set<Vertex> vertices, Graph g, std::set<Edge> blackEdges, float epsilon, vector<Graph> connectedComponents){
    int numOfBlackEdges = 0; //pocet ciernych hran medzi vrcholmi vo vertices
    for (auto &rot: g){
        if (vertices.find(rot.v())!=vertices.end()){ //berieme vrcholy iba z mnoziny vertices
            for (auto it: g[rot.n()].list(IP::primary())){ //prejdeme incidentne hrany vrchola
                if (blackEdges.find(it->e())!=blackEdges.end()){
                    numOfBlackEdges++;
                }
            }
        }

    }
    return vertices.size()-1-numOfBlackEdges/2; //numOfBlackEdges treba zmensit, lebo kazdu hranu zaratame dvakrat


}

/*
* vyhadzujeme male sety v ktorych sa nachadzaju cierne cykly
*/
void step1(Graph g, std::set<Edge> blackEdges, float epsilon, vector<Graph> &connectedComponents){
    for (auto &comp: connectedComponents){
        vector<Vertex> compVertices = comp.list(RP::all(), RT::v());
        if (isLarge(compVertices, g, blackEdges, epsilon)){
        //if (g.order()<=20/epsilon+41){
            int b1 = 0, b3 = 0;
            for (auto &rot1: comp){
                int pocetCiernychHran = 0;
                for(auto it: comp[rot1.n()].list(IP::primary()))
                    if(comp.contains(it->n2()))
                        pocetCiernychHran++;
                assert((pocetCiernychHran>0)&&(pocetCiernychHran<=3));
                if (pocetCiernychHran==1)
                    b1++;
                if (pocetCiernychHran==3)
                    b3++;
            }
            if (b1<=b3)
                connectedComponents.erase(remove(connectedComponents.begin(), connectedComponents.end(), comp));
        }
    }
}

//funkcia vyhodi komponent toErase a aj vsetky male komponenty s nim spojene cervenou hranou
void eraseLargeThickComp(Graph &g, std::set<Edge> blackEdges, float epsilon, vector<Graph> &connectedComponents, Graph &toErase){
    for(auto &comp: connectedComponents){
        vector<Vertex> compVertices = comp.list(RP::all(), RT::v());
        if (!isLarge(compVertices, g, blackEdges, epsilon)){ //v malych setoch prejdem vrcholy a zistim ci nemaju cervenu hranu s toErase. ak hej, tak komponent vyhodim
            for (auto &rot: comp){
                for (auto it: g[rot.n()].list(IP::primary())){
                    if ((blackEdges.find(it->e())==blackEdges.end())&&(toErase.contains(it->v2()))){
                        connectedComponents.erase(remove(connectedComponents.begin(), connectedComponents.end(), comp));
                    }
                }
            }
        }
    }
    connectedComponents.erase(remove(connectedComponents.begin(), connectedComponents.end(), toErase));
}

/*
 * vyhadzujeme velke tenke sety aj s malymi setmi cervenou hranou s nimi spojenymi
 */
void step2(Graph g, std::set<Edge> blackEdges, float epsilon, vector<Graph> &connectedComponents){
    //z rodiny komponentov budeme vyhadzovat prvky pokym tam budu, pricom vyhadzoavnim sa mozu zmenit tenke mnoziny
    // na hrube a naopak, preto prehladavanie ukoncime az ked od posledneho najdenia neprejdeme vsetky prvky

    //int esteTrebaPrejst = connectedComponents.size();

    while(true){
        bool vyhodiliSme = false;
        for (auto &comp: connectedComponents){
            // pokial sme pocas celeho for cyklu nevyhodili nic, prejdeme do riadku kde brejkneme while cyklus.
            // pokial sme vyhodili, tak v tom momente sa vraciame na zaciatok a robime cely for cyklus od zaciatku
            vector<Vertex> compVertices = comp.list(RP::all(), RT::v());
            if ((isLarge(compVertices, g, blackEdges, epsilon))&&(!isThin(compVertices, g, blackEdges, epsilon, connectedComponents))){
                //vyhadzovanie komponentov bude prebiehat nasledovne
                //spravim identicku kopiu IK grafu G
                //v IK sa budem pozerat ktore komponenty treba vyhodit, v G ich budem vyhadzovat
                //po kazdej iteracii aktualizujem IK, nech IK==G

                //zavolam funkciu eraseLargeThickComp ktora okrem neho vyhodi aj vsetky male susedne cez cervenu hranu
                eraseLargeThickComp(g, blackEdges, epsilon, connectedComponents, comp);
                vyhodiliSme = true;
            }
        }
        if (!vyhodiliSme)
            break;
    }

}

//vymazeme vrcholy stupna 1 az ziaden taky nezostane
void reducing1(Graph &g, Rotation &rot, Graph &removedGraph, map<Vertex, int> &visited){
    visited[rot.v()] = true;
    if (rot.neighbours().size() > 1) {
        for (auto &it: rot) {
            if (!visited[it.v2()])
                reducing1(g, it.r2(), removedGraph, visited);
        }
    }
    if (rot.neighbours().size() == 1) {
        addV(removedGraph, rot.v());
        addV(removedGraph, rot[0].v2());
        addE(removedGraph, Location(rot[0].n1(), rot[0].n2())); //pozor, pridavam incidenciu k zatial neexistujucej rotacii
        deleteV(g, rot.v());
    }
}

//T(e) si budeme pamatat v mape - z vrcholov do hran

//vsetky cesty vrcholov stupna 2 zmrstime do jednej hrany. t.j.
//TODO:neviem ci ma medzi vrcholmi stupna 3 zostat vrchol, ci dokonca dva
void reducing2(Graph &g, Graph &removedGraph, Rotation &rot, vector<Vertex> &toReduceV, vector<Edge> &toReduceE){
    if (rot.neighbours().size()==2){
        if (find(toReduceV.begin(), toReduceV.end(), rot.v())==toReduceV.end())
            toReduceV.push_back(rot.v());
        if (find(toReduceE.begin(), toReduceE.end(), rot[0].e())==toReduceE.end()) {
            toReduceE.push_back(rot[0].e());
            toReduceV.push_back(rot[0].v2());
        }
        if (find(toReduceE.begin(), toReduceE.end(), rot[1].e())==toReduceE.end()) {
            toReduceE.push_back(rot[1].e());
            toReduceV.push_back(rot[1].v2());
        }
        reducing2(g, removedGraph, rot[0].r2(), toReduceV, toReduceE);
        reducing2(g, removedGraph, rot[1].r2(), toReduceV, toReduceE);
    }
    if (rot.neighbours().size()==3){
        toReduceV.erase(find(toReduceV.begin(), toReduceV.end(), rot.v()));
    }
}

void reducedGraph(vector<Graph> &components, set<Edge> blackEdges, float epsilon){
    map<Edge, Graph> edgeGraph;
    Factory f;
    Graph removedGraph(createG(f));
    for (auto &comp: components){
        for (auto &rot: comp){ //chcem pristupit k lubovolnemu vrcholu
            std::map<Vertex, int> visited;
            for(auto &rot: comp) visited[rot.v()]=0;
            reducing1(comp, rot, removedGraph, visited);
            break;
        }
        vector<Vertex> toReduceV;
        vector<Edge> toReduceE;
        reducing2(comp, removedGraph, comp[0], toReduceV, toReduceE);
        //TODO: toReduceV a toReduceE pridat do removedGraph, odstranit z comp a namiesto toho do comp pridat
        // hranu ktora bude mapou odkazovat na removedGraph
    }
}

bool isFat(std::set<Vertex> vertices, Graph &g, set<Edge> blackEdges, float epsilon, vector<Graph> connectedComponents){
    float delta = epsilon/(1+2*epsilon);
    return s(vertices, g, blackEdges, epsilon, connectedComponents)>
            (4/(epsilon/(1+2*epsilon)))*alfa(vertices, g, blackEdges, epsilon, connectedComponents);
}

//sum of alfas of non-fat edges (components represented by edge in reduced graph) incident
int n(){

    return 0;
}



















