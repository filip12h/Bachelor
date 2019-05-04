#include <vector>
#include "basic_impl.hpp"
#include <assert.h>
#include <algorithm>
#include "redBlackEdges.h"
#include <iostream>


using namespace ba_graph;
using namespace std;


struct node
{
    Number vertex;
    struct node *left;
    struct node *right;
    struct node *parent;
    bool isDesignated;
};

//unique_ptr

struct node* createNode(Number vertex){
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->vertex = vertex;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;
    newNode->isDesignated = false;

    return newNode;
}

//funkcia na vratenie pamate free()

struct node* insertLeft(struct node *root, Number vertex) {
    root->left = createNode(vertex);
    root->left->parent = root;
    return root->left;
}

struct node* insertRight(struct node *root, Number vertex){
    root->right = createNode(vertex);
    root->right->parent = root;
    return root->right;
}

//tuto budem riesit lemmu 1

void DFS(Number n, map<Number, int> &visited, Graph &g, set<Number> &vertices) {
    // oznac vrchol ako navstiveny a pridaj ho do komponentu
    visited[n] = true;
    vertices.insert(n);

    //kazdy nenavstiveny susedny vrchol pridaj do komponentu aj s hranou ktorou sme ho objavili
    for (auto &inc: g[n]){
        Number neighbor = inc.n2();
        if (!visited[neighbor]){
            vertices.insert(neighbor);
            visited[neighbor] = true;
            DFS(neighbor, visited, g, vertices);
        }
    }
}

void DFS2(Number n, map<Number, int> &visited, Graph &graph,
        set<Number> g, set<Number> &vertices, map<Number, bool> &isRemoved){
    // oznac vrchol ako navstiveny a pridaj ho do komponentu
    visited[n] = true;
    vertices.insert(n);

    //kazdy nenavstiveny susedny vrchol pridaj do komponentu aj s hranou ktorou sme ho objavili
    for (auto &inc: graph[n]){
        Number neighbor = inc.n2();
        //pokial dany sused vrcholu z povodneho grafu je este nenavstiveny,
        // nie je oznaceny ako vymazany a zaroven este nepatri do mnoziny vertitces
        if (((!visited[neighbor])&&(!isRemoved[neighbor]))&&(vertices.find(neighbor)==vertices.end()))
            DFS2(neighbor, visited, graph, g, vertices, isRemoved);
    }
}


void DFS0(const Rotation &rot, map<Number, int> &visited, Graph &g, Factory &f, Graph &graph) {
    visited[rot.n()] = true;
    addV(graph, rot.n(), f);
    for (auto &inc: rot) {
        Rotation& neighbor = inc.r2();
        if (!visited[neighbor.n()]) {
            addV(graph, neighbor.n(), f);
            visited[neighbor.n()] = true;
            DFS0(neighbor, visited, g, f, graph);
        }
    }
}
vector<Graph> getComponents(Graph &g, Factory f) {
    vector<Graph> components;
    std::map<Number, int> visited;
    for(auto &rot: g) visited[rot.n()]=0;
    Graph graph(createG(f));
    for (auto &rot: g)
        if (!visited[rot.n()]) {
            DFS0(rot, visited, g, f, graph);
            for(auto &rot1: graph)
                for(auto it: g[rot1.n()].list(IP::primary()))
                    if(graph.find(it->n2())!=graph.end())
                        addE(graph, rot1, it->r2(), it->e(), f);
            components.push_back(std::move(graph));
        }
    return components;
}

// metoda vrati komponenty grafu
set<set<Number>> connectedComponents(Graph &g) {
    set<set<Number>> components;
    map<Number, int> visited;
    for(auto &rot: g) visited[rot.n()]=0;

    //najdeme nenavstiveny vrchol a nasledne najdeme hrany a vrcholy s nim spojene
    for (auto &rot: g) {
        set<Number> vertices;
        if (!visited[rot.n()]) {
            DFS(rot.n(), visited, g, vertices);
            components.insert(vertices);
        }
    }
    return components;
}

//isRemoved nekoresponduje s isRemoved v redukcii grafu, tuto mame oznacene vrcholy isRemoved tie,
// ktore nepatria do vymazanych stromov. Paradox.
set<set<Number>> connectedComponents(Graph &graph, set<Number> g, map<Number, bool> &isRemoved){
    set<set<Number>> components;
    map<Number, int> visited;
    for(auto &n: g) visited[n]=0;

    //najdeme nenavstiveny vrchol a nasledne najdeme hrany a vrcholy s nim spojene
    for (auto &n: g) {
        if ((!visited[n])&&(!isRemoved[n])) {
            set<Number> vertices;
            DFS2(n, visited, graph, g, vertices, isRemoved);
            components.insert(vertices);
        }
    }
    return components;
}

//probably unusefull
set<set<Number>> blackComponents(Graph g, set<pair<Number, Number>> blackEdges){
    for (auto &rot:g){
        for(auto &inc:g[rot.n()]){
            if ((blackEdges.find(pair(inc.n1(), inc.n2()))==blackEdges.end())&&
            (blackEdges.find(pair(inc.n2(), inc.n1()))==blackEdges.end())){
                deleteE(g, inc.e()); //kedze neposielam graf g do funkcie cez referenciu, nemalo by ho to zmenit
            }
        }
    }
    return connectedComponents(g);
}

// set is positive if it has more internal red edges, negative if there are more external black edges, neutral if equal
int isPositive(set<Number> vertices, Graph &g, set<pair<Number, Number>> blackEdges){
    int intRedEdges = 0, extBlackEdges = 0;
    for (auto &n: vertices){
        for (auto &inc: g[n]){
            if (((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
            (blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end()))&&(vertices.find(inc.n2()) == vertices.end()))
                extBlackEdges++;
            else if (((blackEdges.find(pair(inc.n1(), inc.n2()))==blackEdges.end())
            &&(blackEdges.find(pair(inc.n2(), inc.n1()))==blackEdges.end()))&&
            ((vertices.find(inc.n2())!=vertices.end())&&(inc.n1().to_int()<inc.n2().to_int())))//every edge count only once
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
bool isLarge(set<Number> vertices, Graph &g, set<pair<Number, Number>> blackEdges, float epsilon){
    return (vertices.size() > (1+2*epsilon)/epsilon);
}

//optional TODO: opakujuci sa kod vo funkciach alfa() a s() a smallSetsViaRedEdge()

//pocet cervenych hran medzi mnozinou vertices a malymi mnozinami
int alfa(set<Number> &vertices, Graph &g, set<pair<Number, Number>> blackEdges,
        float epsilon, set<set<Number>> &connectedComponents){
    int result = 0;
    for (auto &comp: connectedComponents)
        if (!isLarge(vertices, g, blackEdges, epsilon))
            for (auto &n: vertices)
                for (auto it: g[n].list(IP::primary()))
                    if (((blackEdges.find(pair(it->n1(), it->n2()))==blackEdges.end())&&
                    (blackEdges.find(pair(it->n2(), it->n1()))==blackEdges.end()))&&(comp.find(it->n2()) != comp.end()))
                        result ++;
    return result;
}

//zjednotenie mnoziny vertices s malymi setmi spojenych cervenou hranou
//addOnlyOneSet is on when simple cases such that 2 small sets connected via red edge
set<Number> smallSetsViaRedEdge(set<Number> vertices, Graph &g, set<pair<Number, Number>> blackEdges, float epsilon,
                                set<set<Number>> &connectedComponents, bool addOnlyOneSet) {
    set<Number> addedVertices;
    for (auto &n: vertices) addedVertices.insert(n);

    for (auto &comp: connectedComponents){
        bool compFound = false;
        if (!isLarge(vertices, g, blackEdges, epsilon)) //zoberieme maly komponent
            for (auto &n: comp){ //prechadzame vrcholmi nasej mnoziny
                for (auto it: g[n].list(IP::primary())){ //prejdeme hranami daneho vrchola
                    if (((blackEdges.find(pair(it->n1(), it->n2()))==blackEdges.end())&&
                    (blackEdges.find(pair(it->n2(), it->n1()))==blackEdges.end()))
                    &&((vertices.find(it->n2()))!=vertices.end())){
                        //pokial je dana hrana cervena a zaroven na druhej strane je vrchol z komponentu
                        for (auto &vertexToAdd: comp)
                            addedVertices.insert(vertexToAdd);
                        compFound = true;
                        break;
                        //po najdeni daneho komponentu chcem vyliezt z for cyklov for(it) aj for (n) a ist na dalsi
                        // komponent v poradi
                    }
                }
                if (compFound) break;
            }
        if (compFound && addOnlyOneSet)
            break;
    }
    return addedVertices;
}

//velkost zjednotenia mnoziny vertices s malymi mnozinami spojenymi cervenou hranou
int s(set<Number> &vertices, Graph &g, set<pair<Number, Number>> blackEdges,
        float epsilon, set<set<Number>> connectedComponents){
    int result = vertices.size();
    for (auto &comp: connectedComponents){
        bool compFound = false;
        if (!isLarge(vertices, g, blackEdges, epsilon)) //zoberieme maly komponent
            for (auto &n: vertices){ //prechadzame vrcholmi nasej mnoziny
                for (auto it: g[n].list(IP::primary())){ //prejdeme hranami daneho vrchola
                    if (((blackEdges.find(pair(it->n1(), it->n2()))==blackEdges.end())&&
                    (blackEdges.find(pair(it->n2(), it->n1()))==blackEdges.end()))&&((comp.find(it->n2()))!=comp.end())){
                        //pokial je dana hrana cervena a zaroven na druhej strane je vrchol z komponentu
                        result += comp.size();
                        compFound = true;
                        break;
                        //po najdeni daneho komponentu chcem vyliezt z for cyklov for(it) aj for (n) a ist na
                        // dalsi komponent v poradi
                    }
                }
                if (compFound) break;
            }
    }
    return result;
}

bool isThin(set<Number> vertices, Graph &g, set<pair<Number, Number>> &blackEdges,
        float epsilon, set<set<Number>> connectedComponents){
    return s(vertices, g, blackEdges, epsilon, connectedComponents)<=
           (alfa(vertices, g, blackEdges, epsilon, connectedComponents)+1)/(epsilon/(1+2*epsilon));

}

//strom ma n-1 hran, tie zvysne su vyhoditelne
//number of edges which can be removed without splitting it into 2 or more components
//kolko cyklov tam je, tolko hran vieme vyhodit
/*
 * pre spravne fungovanie musi byt mnozina vertices niektory jeden cely cierny komponent
 */
int r(set<Number> vertices, Graph g, set<pair<Number, Number>> blackEdges,
        float epsilon, set<set<Number>> connectedComponents){
    int numOfBlackEdges = 0; //pocet ciernych hran medzi vrcholmi vo vertices
    for (auto &n: vertices){
        for (auto it: g[n].list(IP::primary())){ //prejdeme incidentne hrany vrchola
            if ((blackEdges.find(pair(it->n1(), it->n2()))!=blackEdges.end())||
            (blackEdges.find(pair(it->n2(), it->n1()))!=blackEdges.end())){
                numOfBlackEdges++;
            }
        }

    }
    return vertices.size()-1-numOfBlackEdges/2; //numOfBlackEdges treba 2x zmensit, lebo kazdu hranu zaratame dvakrat


}

/*
* vyhadzujeme male sety v ktorych sa nachadzaju cierne cykly
*/
void step1(Graph &g, set<pair<Number, Number>> blackEdges, float epsilon, set<set<Number>> &connectedComponents,
           map<Number, bool> &isRemoved){
    for (auto &comp: connectedComponents){
        if (!isLarge(comp, g, blackEdges, epsilon)){
            int b1 = 0, b3 = 0;
            for (auto &n: comp){
                int pocetCiernychHran = 0;
                for(auto &inc: g[n])
                    if((comp.find(inc.n2())!=comp.end())&&((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())
                    ||(blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end())))
                        pocetCiernychHran++;
                assert((pocetCiernychHran>0)&&(pocetCiernychHran<4));
                if (pocetCiernychHran==1)
                    b1++;
                if (pocetCiernychHran==3)
                    b3++;
            }
            if (b1<=b3)
                for (auto &n: comp)
                    isRemoved[n] = true;
        }
    }
//
//    auto isErased = [&](std::set<Number> conC){return erasedComponents.count(conC)>0;};
//
//    for(auto it=connectedComponents.begin(); it!=connectedComponents.end(); it++)
//        if (isErased(*it))
//            it = connectedComponents.erase(it);
}

//funkcia vyhodi komponent toErase a aj vsetky male komponenty s nim spojene cervenou hranou
void eraseLargeThickComp(Graph &g, set<pair<Number, Number>> blackEdges, float epsilon,
        set<set<Number>> &connectedComponents, set<Number> toErase, map<Number, bool> &isRemoved){

    for (auto &comp: connectedComponents)
        if (!isLarge(comp, g, blackEdges, epsilon))
            //v malych setoch prejdem vrcholy a zistim ci nemaju cervenu hranu s toErase. ak hej, tak komponent vyhodim
            for (auto &n: comp)
                for (auto it: g[n].list(IP::primary()))
                    if (((blackEdges.find(pair(it->n1(), it->n2())) == blackEdges.end())&&
                    (blackEdges.find(pair(it->n2(), it->n1()))==blackEdges.end()))&&(toErase.find(it->n2())!=toErase.end())
                    &&(!isRemoved[it->n2()]))
                        for (auto &m: comp)
                            isRemoved[m] = true;
}

/*
 * vyhadzujeme large thick sety aj s malymi setmi cervenou hranou s nimi spojenymi
 */
void step2(Graph &g, set<pair<Number, Number>> blackEdges, float epsilon, set<set<Number>> &connectedComponents,
           map<Number, bool> &isRemoved){
    //z rodiny komponentov budeme vyhadzovat prvky pokym tam budu, pricom vyhadzoavnim sa mozu zmenit tenke mnoziny
    // na hrube a naopak, preto prehladavanie ukoncime az ked od posledneho najdenia neprejdeme vsetky prvky

    //int esteTrebaPrejst = connectedComponents.size();

    while(true){
        bool vyhodiliSme = false;
        for (auto &comp: connectedComponents){
            // pokial sme pocas celeho for cyklu nevyhodili nic, prejdeme do riadku kde brejkneme while cyklus.
            // pokial sme vyhodili, tak v tom momente sa vraciame na zaciatok a robime cely for cyklus od zaciatku
            if ((isLarge(comp, g, blackEdges, epsilon))&&(!isThin(comp, g, blackEdges, epsilon, connectedComponents))){
                //vyhadzovanie komponentov bude prebiehat nasledovne
                //spravim identicku kopiu IK grafu G
                //v IK sa budem pozerat ktore komponenty treba vyhodit, v G ich budem vyhadzovat
                //po kazdej iteracii aktualizujem IK, nech IK==G

                //zavolam funkciu eraseLargeThickComp ktora okrem neho vyhodi aj vsetky male susedne cez cervenu hranu
                eraseLargeThickComp(g, blackEdges, epsilon, connectedComponents, comp, isRemoved);
                vyhodiliSme = true;
            }
        }
        if (!vyhodiliSme)
            break;
    }

}

//fat is edge which refers to set vertices
bool isFat(map<pair<Number, Number>, set<Number>> &extraEdges, pair<Number, Number> edge, Graph &g,
        set<pair<Number, Number>> &blackEdges, float epsilon, set<set<Number>> connectedComponents){
    set<Number> vertices;
    if (extraEdges.find(edge)!=extraEdges.end()) {
        vertices = extraEdges.at(edge);
    }
    if (extraEdges.find(pair(edge.second, edge.first))!=extraEdges.end()){
        vertices = extraEdges.at(pair(edge.second, edge.first));
    } //kedze neorientovana hrana moze byt ulozena dvojakym sposobom
    return s(vertices, g, blackEdges, epsilon, connectedComponents)>
           (4/(epsilon/(1+2*epsilon)))*alfa(vertices, g, blackEdges, epsilon, connectedComponents);
}

//sum of alfas of non-fat edges (components represented by edge in reduced graph) incident to vertex
int n(Number vertex, map<pair<Number, Number>, set<Number>> &extraEdges, Graph &g,
        set<pair<Number, Number>> &blackEdges, float epsilon, set<set<Number>> connectedComponents){
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
map<pair<Number, Number>, set<Number>> edgeRef(Graph &graph, map<Number, bool> &isRemoved,
        set<pair<Number, Number>> blackEdges){
    map<pair<Number, Number>, set<Number>> er;
    set<set<Number>> components = connectedComponents(graph);
    //int edgeCounter = 0;
    map<Number, bool> isNotRemoved;
    for (int i = 0; i<graph.order(); i++)
        isNotRemoved[i] = !isRemoved[i];

    for (auto &comp: components){
//        set<Number> toDelete; //mnozina vrcholov ktore z comp odstranim
//        for (auto &n: comp)
//            if (!isRemoved[n.to_int()]) //take vrcholy ktore sme neoznacili ako vymazane teraz vymazeme
//                toDelete.insert(n);

        //kernelOfComponent is set of vertices which were not reduced
        set<Number> kernelOfComponent = connectedComponents(graph, comp, isRemoved).begin().operator*();


        if (!kernelOfComponent.empty()){
            set<set<Number>> reducedTreesOfComponent = connectedComponents(graph, comp, isNotRemoved);
            //TODO: is this working properly?!?!?!?!??!?!?!?!?!??!?!?!!?!!??!?!?!??!?!?!?!??!?!?!?!?
            for (auto &rtc: reducedTreesOfComponent){
                vector<Number> vertices;
                //kazdy komponent bude predstavovany jednou hranou - z vrchola v1 do v2(moze to byt aj ten isty vrchol)
                set<Number> allVerticesInComponent; //komponent budu tvorit len vrcholy
                for (auto &n: rtc){
                    for (auto &inc: graph[n])
                        if (((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
                             (blackEdges.find(pair(inc.n2(), inc.n1())) != blackEdges.end())) &&
                             (kernelOfComponent.find(inc.n2())!=kernelOfComponent.end()))
                            vertices.push_back(inc.n2());
                    allVerticesInComponent.insert(n);
                }
                er.insert(make_pair(make_pair(vertices[0], vertices[1]), allVerticesInComponent));
            }

//            for (auto &n: rc) {
//                for (auto &inc: graph[n])
//                    if (((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
//                    (blackEdges.find(pair(inc.n2(), inc.n1())) != blackEdges.end())) && (!isRemoved[inc.n2()]))
//                        vertices.push_back(inc.n2());
//                allVerticesInComponent.insert(n);
//            }
//            er.insert(make_pair(make_pair(vertices[0], vertices[1]), allVerticesInComponent));
            //chcel by som na "vymazane" grafy odkazovat cez hranu, nie dvojicu vrcholov
        }
    }
    return er;
}

void removeLine(Graph &g, set<Number> &graphToReduce, map<Number, bool> &isRemoved,
        const Number n, set<pair<Number, Number>> &blackEdges){
    if (!isRemoved[n]){
        int removedNeighbors = 0, numOfBlackEdges = 0;
        for (auto &inc: g[n]) {
            if ((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
                    (blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end())) {
                numOfBlackEdges++;
                if (isRemoved[inc.n2()])
                    removedNeighbors++;
            }
        }
        //set as removed only if it has 1 or no neighbor
        if (numOfBlackEdges - removedNeighbors < 2) {
            isRemoved[n] = true;
            for (auto &inc: g[n])
                if ((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
                        (blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end()))
                    if (graphToReduce.find(inc.n2())!=graphToReduce.end())
                        removeLine(g, graphToReduce, isRemoved, inc.n2(), blackEdges);
        }
    }
}

//vyhodime vsetky vrcholy stupna 1 v ciernych komponentoch az ziaden taky nezostane
void reduceStep1(Graph &g, set<Number> graphToReduce, map<Number, bool> &isRemoved,
        set<pair<Number, Number>> &blackEdges){
    for (auto &n: graphToReduce){
        removeLine(g, graphToReduce, isRemoved, n, blackEdges);
    }
}

//v komponente zo zvysnych vrcholov nechame iba vrcholy stupna 3,
//cesty edzi nimi nahradime jednou hranou (vo funkcii edgeRef)
void reduceStep2(Graph &g, set<Number> graphToReduce,  map<Number, bool> &isRemoved,
        set<pair<Number, Number>> &blackEdges) {
    bool toRemove[g.order()];
    for (auto &a: toRemove)
        a = false;
    for (auto &n: graphToReduce) {
        int removedNeighbors = 0, numOfBlackEdges = 0;
        for (auto &inc: g[n])
            if ((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
                    (blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end())) {
                numOfBlackEdges++;
                if (isRemoved[inc.n2()])
                    removedNeighbors++;
            }
        if (numOfBlackEdges - removedNeighbors < 3) // == 2
            toRemove[n.to_int()] = true;
    }
    //najprv som si vymazanie dvojkovych zaznacil, az potom naraz som ich vymazal,
    //lebo inak by som vymazal prakticky vsetky vrhcoly
    for (int a = 0; a<g.order(); a++){
        if (toRemove[a]){
            isRemoved[a] = true;
        }
    }
}

//
node* createSubtree(Graph &graph, node* &root, set<Number> &vertices, set<pair<Number, Number>> edges){
    for (auto &inc: graph[root->vertex])
        //aby sme sa z vrchola pozerali do podstromu a po spravnej hrane
        if ((vertices.find(inc.n2())!=vertices.end())&&((edges.find(pair(inc.n1(), inc.n2()))!=edges.end())||
                (edges.find(pair(inc.n2(), inc.n1()))!=edges.end()))) {
            if (root->parent->vertex != inc.n2()) { //pokial sa pozeram na vrchol ktory je mojim otcom tak nic nerob
                node* newNode = createNode(inc.n2());
                if (root->left == NULL)
                    insertLeft(root, newNode->vertex);
                else insertRight(root, newNode->vertex);
                createSubtree(graph, newNode, vertices, edges);
            }
        }
    return root;
}

set<Number> getSubtreeVertices(Graph &graph, node* root, set<Number> &subtreeVertices){
    subtreeVertices.insert(root->vertex);
    if (root->left!=NULL) getSubtreeVertices(graph, root->left, subtreeVertices);
    if (root->right!=NULL) getSubtreeVertices(graph, root->right, subtreeVertices);
    return subtreeVertices;
}

bool isNodeInSubtree(Graph &graph, node* root, Number wanted){
    if (wanted.to_int()==root->vertex.to_int()) return true;
    if (root==NULL) return false;
    return (isNodeInSubtree(graph, root->left, wanted)||(isNodeInSubtree(graph, root->right, wanted)));
}

void undesignateNodesAbove(node* root){
    while (root->parent!=NULL){
        root->parent->isDesignated=false;
        undesignateNodesAbove(root->parent);
    }
}

void designateNodes(Graph &graph, node* root, set<pair<Number, Number>> blackEdges,
        float epsilon, set<set<Number>> components, Number marked1, Number marked2){
    if ((!isNodeInSubtree(graph, root, marked1))&&(!isNodeInSubtree(graph, root, marked2))) {
        set<Number> subtreeVertices = getSubtreeVertices(graph, root, subtreeVertices);
        if (alfa(subtreeVertices, graph, blackEdges, epsilon, components) == 2) {
            root->isDesignated = true;
            undesignateNodesAbove(root);
        }
    }
    if (root->left!=NULL)
        designateNodes(graph, root->left, blackEdges, epsilon, components, marked1, marked2);
    if (root->right!=NULL)
        designateNodes(graph, root->right, blackEdges, epsilon, components, marked1, marked2);

}

set<node*> getDesignatedNodes(node* root, set<node*> &nodes){
    if (root->isDesignated) nodes.insert(root);
    if (root->left!=NULL) getDesignatedNodes(root->left, nodes);
    if (root->right!=NULL) getDesignatedNodes(root->right, nodes);
    return nodes;
}

//function creates reducedGraph from graph - reduceStep1 and reduceStep2
set<Number> reducedGraph(Graph &graph, set<pair<Number, Number>> &blackEdges, float epsilon, map<Number, bool> &isRemoved,
        set<set<Number>> blackComponents, Graph &blackGraph, Factory &f){
    for (auto &comp: blackComponents){
        reduceStep1(graph, comp, isRemoved, blackEdges);
        reduceStep2(graph, comp, isRemoved, blackEdges);
    }
    map<pair<Number, Number>, set<Number>> extraEdges = edgeRef(blackGraph, isRemoved, blackEdges);

    Graph rg(createG(f)); //create reduced graph rg
    for (auto &rot: graph) // add all remaining (not removed) vertices in graph to rg
        if (!isRemoved[rot.n()]) {
            addV(rg, rot.n());
            for (auto &inc: rot) //add black edges between added vertices to rg
                if ((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
                (blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end())) //only if edge is not added yet
                    addE(rg, inc.e());
        }
    for (auto &edge: extraEdges) { //add edges between extra verices - these edges reference to removed subgraphs
        //najdi vrchol v1 z extra hrany v mnozine, nasledne najdi vrchol v2 extra hrany a hranu pridaj
        addE(rg, rg.find(edge.first.first).operator*().v(), rg.find(edge.first.second).operator*().v());
    }
    set<Number> result;
    for (auto &rot: rg){
        if (n(rot.n(), extraEdges, graph, blackEdges, epsilon, blackComponents)==4){
            result.insert(rot.n());
            for (auto &inc: rot)
                if (!isFat(extraEdges, pair(inc.n1(), inc.n2()), graph, blackEdges, epsilon, blackComponents))
                    for (auto &n:extraEdges.at(pair(inc.n1(), inc.n2())))
                        result.insert(n);

            set<Number> zjednotenie = smallSetsViaRedEdge(result, graph, blackEdges, epsilon, blackComponents, false);
            for (auto &z: zjednotenie)
                if (result.find(z)==result.end())
                    result.insert(z);
            return result;
        }
        if (n(rot.n(), extraEdges, graph, blackEdges, epsilon, blackComponents)>4){
            int numOfAlfa2Edges = 0;
            set<set<Number>> treesOfAlfa2Edges;
            for (auto &inc: rot) {
                set<Number> vertices;
                if (extraEdges.find(pair(inc.n1(), inc.n2())) != extraEdges.end())
                    vertices = extraEdges.at(pair(inc.n1(), inc.n2()));
                if (extraEdges.find(pair(inc.n2(), inc.n1())) != extraEdges.end())
                    vertices = extraEdges.at(pair(inc.n2(), inc.n1()));
                if ((!isFat(extraEdges, pair(inc.n1(), inc.n2()), graph, blackEdges, epsilon, blackComponents))
                    && (alfa(vertices, graph, blackEdges, epsilon, blackComponents)==2)) {
                    numOfAlfa2Edges++; //podmienka na to aby result splnal lemu
                    treesOfAlfa2Edges.insert(vertices);
                }
            }
            if (numOfAlfa2Edges==2) {
                result.insert(rot.n());
                for (auto &tree: treesOfAlfa2Edges)
                    for (auto &n:tree)
                        result.insert(n);
            }
            return result;
        }
    }
    //v redukovanom grafe najdeme netucnu hranu s alfa>=3
    for (auto &rot: rg){
        for (auto &inc: rot){
            set<Number> vertices;
            Number v1, v2;
            if (extraEdges.find(pair(inc.n1(), inc.n2())) != extraEdges.end()) {
                vertices = extraEdges.at(pair(inc.n1(), inc.n2()));
                v1 = inc.n1();
                v2 = inc.n2();
            }
            if (extraEdges.find(pair(inc.n2(), inc.n1())) != extraEdges.end()) {
                vertices = extraEdges.at(pair(inc.n2(), inc.n1()));
                v1 = inc.n2();
                v2 = inc.n1();
            }

            if ((!isFat(extraEdges, pair(inc.n1(), inc.n2()), graph, blackEdges, epsilon, blackComponents))&&
            (alfa(vertices, graph, blackEdges, epsilon, blackComponents)>=3)){
                //oznacime 2 vrcholy spojene cez externe cierne hrany
                //TODO: K is a tree - pokial 2 externe hrany odkazuju na 1 vrchol tak nie
                set<Number> k;
                for (auto &n: vertices)
                    k.insert(n);
                k.insert(v1);
                k.insert(v2);

                struct node* root = createNode(v1);

                createSubtree(graph, root, k, blackEdges);

                designateNodes(graph, root, blackEdges, epsilon, blackComponents, v1, v2);

                set<node*> nodes;

                nodes = getDesignatedNodes(root, nodes);

                for (auto &nd: nodes){
                    set<Number> subtreeVertices = getSubtreeVertices(graph, nd, subtreeVertices);
                    if (s(subtreeVertices, graph, blackEdges, epsilon, blackComponents)<=20/(epsilon/(1+2*epsilon)))
                        return smallSetsViaRedEdge(subtreeVertices, graph, blackEdges, epsilon, blackComponents, false);
                }

            }
        }
    }

    return result;
}

/*
* funkcia z grafu, v ktorom su cervene a cierne hrany take, ze kazdeho vrcholu sa dotyka aspon jedna cierna hrana
* a zaroven nech je cervenych hran viac ako ciernych.
* Najdeme mnozinu urcitej/obmedzenej velkosti taku, ze cervenych interncyh hran je viac ako ciernych externych
*/
set<Number> redBlackEdges(Graph &g, set<pair<Number, Number>> &blackEdges, float epsilon, Graph &ciernyGraf, Factory &f) {

    set<set<Number>> rodinaCiernychKomponentov = connectedComponents(ciernyGraf);

    //z rodinaCiernychKomponentov budeme neskor vyhadzovat jednotlive elementy, zostane tam velka tenka
    //mnozina s nejakymi vlastnostami a nasledne najdeme mnozinu splnajucu lemu

    //some simple cases such that small positive set or 2 (or more) small sets connected via a red edge
//    for (auto &comp: rodinaCiernychKomponentov){
//        if (isPositive(comp, g, blackEdges))
//            return comp;
//        else if (isPositive(smallSetsViaRedEdge(comp, g, blackEdges, epsilon, rodinaCiernychKomponentov, true), g, blackEdges))
//            return smallSetsViaRedEdge(comp, g, blackEdges, epsilon, rodinaCiernychKomponentov, true);
//    }


    map<Number, bool> isRemoved;
    for (int i = 0; i<g.order(); i++)
        isRemoved[i] = false;

    //step1(g, blackEdges, epsilon, rodinaCiernychKomponentov, isRemoved);

    step2(g, blackEdges, epsilon, rodinaCiernychKomponentov, isRemoved);

    //cast was offered by CLion, without casting no building was able
    //TODO: add as parameter also rodinaCiernychKomponentov?
    return reducedGraph(g, blackEdges, epsilon, isRemoved,
            rodinaCiernychKomponentov, ciernyGraf, f);

}
