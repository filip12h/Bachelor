#include <vector>
#include <basic_impl.hpp>
#include <assert.h>
#include <algorithm>


using namespace ba_graph;
using namespace std;

//tuto budem riesit lemmu 1

void DFS(const Rotation &rot, map<Number, int> &visited, Graph &g, Factory &f, Graph &graph) {
    // oznac vrchol ako navstiveny a pridaj ho do komponentu
    visited[rot.n()] = true;
    addV(graph, rot.n(), f);

    //kazdy nenavstiveny susedny vrchol pridaj do komponentu aj s hranou ktorou sme ho objavili
    for (auto &inc: rot) {
        Rotation& neighbor = inc.r2();
        if (!visited[neighbor.n()]) {
            addV(graph, neighbor.n(), f);
            visited[neighbor.n()] = true;
            DFS(neighbor, visited, g, f, graph);
        }
    }
}

// metoda vrati komponenty grafu
vector<Graph> connectedComponents(Graph &g) {
    vector<Graph> components;
    std::map<Number, int> visited;
    for(auto &rot: g) visited[rot.n()]=0;
    Factory f;
    Graph graph(createG(f));

    //najdeme nenavstiveny vrchol a nasledne najdeme hrany a vrcholy s nim spojene
    for (auto &rot: g) {
        if (!visited[rot.n()]) {
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

//probably unusefull
vector<Graph> blackComponents(Graph g, set<Edge> blackEdges){
    for (auto &rot:g){
        for(auto &inc:rot){
            if (blackEdges.find(inc.e())==blackEdges.end()){
                deleteE(g, inc.e());
            }
        }
    }
    return connectedComponents(g);
}

// set is positive if it has more internal red edges, negative if there are more external black edges, neutral if equal
int isPositive(std::set<Number> vertices, Graph g, std::set<Edge> blackEdges){
    int intRedEdges = 0, extBlackEdges = 0;
    for (auto &rot: g){
        if (vertices.find(rot.n())!=vertices.end()) //overujeme ci dany vrchol rot sa nachadza v nasej mnozine, kedze iba ta nas zaujima
            for (auto it: g[rot.n()].list(IP::primary())){
                //pokial je sused mimo nasej mnoziny a je spojeny ciernou hranou, ta musi byt externa
                if ((blackEdges.find(it->e())!=blackEdges.end())&&(vertices.find(it->n2()) == vertices.end()))
                    extBlackEdges++;
                else if ((blackEdges.find(it->e())==blackEdges.end())&&(vertices.find(it->n2()) != vertices.end()))
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
bool isLarge(std::set<Number> &vertices, Graph &g, std::set<Edge> blackEdges, float epsilon){
    return vertices.size() > (1+2*epsilon)/epsilon;
}

//optional TODO: opakujuci sa kod vo funkciach alfa() a s()

//pocet cervenych hran medzi mnozinou vertices a malymi mnozinami
int alfa(std::set<Number> &vertices, Graph &g, set<Edge> blackEdges, float epsilon, vector<Graph> &connectedComponents){
    int result = 0;
    for (auto &rot: g){
        if (vertices.find(rot.n())!=vertices.end()) //pokial dany vrchol rot je v nasej mnozine vertices
            for (auto it: g[rot.n()].list(IP::primary())){ //prejdeme incidentne hrany vrchola
                //pokial je hrana cervena a vrchol na druhej strane je mimo mnoziny
                if ((blackEdges.find(it->e())==blackEdges.end())&&(vertices.find(it->n2()) != vertices.end()))
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
int s(std::set<Number> &vertices, Graph &g, set<Edge> blackEdges, float epsilon, vector<Graph> connectedComponents){
    int result = vertices.size();
    for (auto &rot: g){
        if (vertices.find(rot.n())!=vertices.end())
            for (auto it: g[rot.n()].list(IP::primary())){
                if ((blackEdges.find(it->e())==blackEdges.end())&&(vertices.find(it->n2()) != vertices.end()))
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

bool isThin(std::set<Number> vertices, Graph g, std::set<Edge> &blackEdges, float epsilon, vector<Graph> connectedComponents){
    return s(vertices, g, blackEdges, epsilon, connectedComponents)<=
           (alfa(vertices, g, blackEdges, epsilon, connectedComponents)+1)/(epsilon/(1+2*epsilon));

}

//strom ma n-1 hran, tie zvysne su vyhoditelne
//number of edges which can be removed without splitting it into 2 or more components
//kolko cyklov tam je, tolko hran vieme vyhodit
/*
 * pre spravne fungovanie musi byt mnozina vertices niektory jeden cely cierny komponent
 */
int r(std::set<Number> vertices, Graph g, std::set<Edge> blackEdges, float epsilon, vector<Graph> connectedComponents){
    int numOfBlackEdges = 0; //pocet ciernych hran medzi vrcholmi vo vertices
    for (auto &rot: g){
        if (vertices.find(rot.n())!=vertices.end()){ //berieme vrcholy iba z mnoziny vertices
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
void step1(Graph &g, std::set<Edge> blackEdges, float epsilon, vector<Graph> &connectedComponents){
    for (auto &comp: connectedComponents){
        vector<Number> compVertices = comp.list(RP::all(), RT::n());
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
        vector<Number> compVertices = comp.list(RP::all(), RT::n());
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
void step2(Graph &g, std::set<Edge> blackEdges, float epsilon, vector<Graph> &connectedComponents){
    //z rodiny komponentov budeme vyhadzovat prvky pokym tam budu, pricom vyhadzoavnim sa mozu zmenit tenke mnoziny
    // na hrube a naopak, preto prehladavanie ukoncime az ked od posledneho najdenia neprejdeme vsetky prvky

    //int esteTrebaPrejst = connectedComponents.size();

    while(true){
        bool vyhodiliSme = false;
        for (auto &comp: connectedComponents){
            // pokial sme pocas celeho for cyklu nevyhodili nic, prejdeme do riadku kde brejkneme while cyklus.
            // pokial sme vyhodili, tak v tom momente sa vraciame na zaciatok a robime cely for cyklus od zaciatku
            vector<Number> compVertices = comp.list(RP::all(), RT::n());
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

//fat is edge which refers to set vertices
bool isFat(map<pair<Number, Number>, set<Number>> &extraEdges, pair<Number, Number> edge, Graph &g, set<Edge> &blackEdges, float epsilon, vector<Graph> connectedComponents){
    set<Number> vertices;
    if (extraEdges.find(edge)!=extraEdges.end()) {
        vertices = extraEdges.at(edge);
    }
    if (extraEdges.find(pair(edge.second, edge.first))!=extraEdges.end()){
        vertices = extraEdges.at(pair(edge.second, edge.first));
    }
    return s(vertices, g, blackEdges, epsilon, connectedComponents)>
           (4/(epsilon/(1+2*epsilon)))*alfa(vertices, g, blackEdges, epsilon, connectedComponents);
}

//sum of alfas of non-fat edges (components represented by edge in reduced graph) incident to vertex
int n(Number vertex, map<pair<Number, Number>, set<Number>> &extraEdges, Graph &g, set<Edge> &blackEdges, float epsilon, vector<Graph> connectedComponents){
    int totalAlfa = 0;

    for (auto &inc: g[vertex])
        if (extraEdges.find(pair(inc.n1(), inc.n2()))!=extraEdges.end()) {
            if (isFat(extraEdges, pair(inc.n1(), inc.n2()), g, blackEdges, epsilon, connectedComponents))
                totalAlfa += alfa(extraEdges.at(pair(inc.n1(), inc.n2())), g, blackEdges, epsilon, connectedComponents);
        } else {
            if (extraEdges.find(pair(inc.n2(), inc.n1()))!=extraEdges.end())
                if (isFat(extraEdges, pair(inc.n2(), inc.n1()), g, blackEdges, epsilon, connectedComponents))
                    totalAlfa += alfa(extraEdges.at(pair(inc.n2(), inc.n1())), g, blackEdges, epsilon, connectedComponents);
        }
    return totalAlfa;
}

//create reference between deleted tree in step 1 a 2 of reducing and the edge which represents it
map<pair<Number, Number>, set<Number>> edgeRef(Graph &graph, bool (&isRemoved)[], set<Edge> blackEdges){
    map<pair<Number, Number>, set<Number>> er;
    vector<Graph> components = connectedComponents(graph);
    int edgeCounter = 0;
    for (auto &comp: components){
        for (auto &rot: comp){
            if (!isRemoved[rot.n().to_int()]) {
                Factory f;
                deleteV(comp, rot.n(), f);
            }
        }
        vector<Graph> reducedComponents = connectedComponents(comp);
        for (auto &comp: reducedComponents){
            vector<Number> vertices; //kazdy komponent bude predstavovany jednou hranou - z vrchola v1 do v2 (moze to byt aj ten isty vrchol)
            set<Number> allVerticesInComponent; //komponent budu tvorit len vrcholy
            for (auto &rot: comp) {
                for (auto &inc: rot)
                    if ((blackEdges.find(inc.e()) != blackEdges.end()) && (!isRemoved[inc.n2().to_int()]))
                        vertices.push_back(inc.n2());
                allVerticesInComponent.insert(rot.n());
            }
            er.insert(make_pair(make_pair(vertices[0], vertices[1]), allVerticesInComponent));
            //chcel by som na "vymazane" grafy odkazovat cez hranu, nie dvojicu vrcholov
        }
    }
    return er;
}

void removeLine(Graph &graph, bool (&isRemoved)[], const Number n){
    if (!isRemoved[n.to_int()]){
        int removedNeighbors = 0;
        for (auto &inc: graph[n])
            if (isRemoved[inc.n2().to_int()])
                removedNeighbors++;
        if (graph[n].neighbours().size() - removedNeighbors == 1) {
            isRemoved[n.to_int()] = true;
            for (auto &inc: graph[n])
                removeLine(graph, isRemoved, inc.n2());
        }
    }
}

void reduceStep1(Graph &graph, bool (&isRemoved)[]){
    for (auto &rot: graph){
        removeLine(graph, isRemoved, rot.n());
    }
}

void reduceStep2(Graph &graph, bool (&isRemoved)[]) {
    bool toRemove[graph.order()];
    for (auto &rot: graph) {
        int removedNeighbors = 0;
        for (auto &inc: graph[rot.n()])
            if (isRemoved[inc.n2().to_int()])
                removedNeighbors++;
        if (graph[rot.n()].neighbours().size() - removedNeighbors < 3) // == 2
            toRemove[rot.n().to_int()] = true;
    }
    //najprv som si vymazanie dvojkovych zaznacil, az potom naraz som ich vymazal, lebo inak by som vymazal prakticky vsetky vrhcoly
    for (auto &a:toRemove){
        if (toRemove[a]){
            isRemoved[a] = true;
        }
    }
}

//function creates reducedGraph from graph - reduceStep1 and reduceStep2
set<Number> reducedGraph(Graph &graph, set<Edge> &blackEdges, float epsilon, bool (&isRemoved)[]){
    Factory f;
    Graph blackComponents(createG(f));
    for (int i = 0; i<graph.order(); i++)
        addV(blackComponents, i, f);
    for(auto &e: blackEdges)
        addE(blackComponents, e, f);
    vector<Graph> components = connectedComponents(blackComponents);
    for (auto &comp: components){
        reduceStep1(comp, isRemoved);
        reduceStep2(comp, isRemoved);
    }
    map<pair<Number, Number>, set<Number>> extraEdges = edgeRef(blackComponents, isRemoved, blackEdges);

    Graph rg(createG(f)); //create reduced graph rg
    for (auto &rot: graph) // add all remaining (not removed) vertices in graph to rg
        if (!isRemoved[rot.n().to_int()]) {
            addV(rg, rot.n());
            for (auto &inc: rot) //add black edges between added vertices to rg
                if (blackEdges.find(inc.e())!=blackEdges.end())
                    addE(rg, inc.e());
        }
    for (auto &edge: extraEdges) { //add edges between extra verices - these edges reference to removed subgraphs
        addE(rg, rg.find(edge.first.first).operator*().v(), rg.find(edge.first.second).operator*().v());
    }

    for (auto &rot: rg){
        if (n(rot.n(), extraEdges, graph, blackEdges, epsilon, components)==4){
            set<Number> result;
            result.insert(rot.n());
            for (auto &inc: rot){
                if (!isFat(extraEdges, pair(inc.n1(), inc.n2()), graph, blackEdges, epsilon, components))
                    for (auto &n:extraEdges.at(pair(inc.n1(), inc.n2())))
                        result.insert(n);
                    //todo pridat este male sety spojene cervenou hranou - tato mnozina splna lemu
            }

            return result;
        }
        if (n(rot.n(), extraEdges, graph, blackEdges, epsilon, components)>4){
            for (auto &inc: rot) {
                set<Number> vertices;
                if (extraEdges.find(pair(inc.n1(), inc.n2())) != extraEdges.end()) {
                    vertices = extraEdges.at(pair(inc.n1(), inc.n2()));
                }
                if (extraEdges.find(pair(inc.n2(), inc.n1())) != extraEdges.end()) {
                    vertices = extraEdges.at(pair(inc.n2(), inc.n1()));
                }
                if ((!isFat(extraEdges, pair(inc.n1(), inc.n2()), graph, blackEdges, epsilon, components))
                    && (alfa(vertices, graph, blackEdges, epsilon, components)){
                    //todo podobne dokoncit ako pripad n(v)=4
                }
            }
            set<Number> result;
            result.insert(rot.n());

        }
    }
}

/*
* funkcia z grafu, v ktorom su cervene a cierne hrany take, ze kazdeho vrcholu sa dotyka aspon jedna cierna hrana
* a zaroven nech je cervenych hran viac ako ciernych.
* Najdeme mnozinu urcitej/obmedzenej velkosti taku, ze cervenych interncyh hran je viac ako ciernych externych
*/
vector<Vertex> redBlackEdges(Graph g, set<Edge> &blackEdges, float epsilon) {
    assert(blackEdges.size()<=g.size()/2); //podla lemy musi byt pocet cervenych hran vacsi ako pocet vrcholov grafu zmenseny o polovicu

    Factory f;
    Graph ciernyGraf(createG(f));
    for(auto &e: blackEdges)
        addE(ciernyGraf, e, f);

    vector<Graph> rodinaCiernychKomponentov = connectedComponents(ciernyGraf);
    //z rodinaCiernychKomponentov budeme neskor vyhadzovat jednotlive elementy, zostane tam velka tenka
    //mnozina s nejakymi vlastnostami a nasledne najdeme mnozinu splnajucu lemu

    //TODO: some simple cases such that small positive set or 2 small sets connected via a red edge

    step1(g, blackEdges, epsilon, rodinaCiernychKomponentov);

    step2(g, blackEdges, epsilon, rodinaCiernychKomponentov);



    //float delta = epsilon/(1+2*epsilon);

}
