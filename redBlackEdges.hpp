#include <vector>
#include "basic_impl.hpp"
#include <assert.h>
#include <algorithm>
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

inline node* createNode(Number vertex){
    auto* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->vertex = vertex;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;
    newNode->isDesignated = false;

    return newNode;
}

//return memory function?

node* insertLeft(struct node *root, Number vertex) {
    root->left = createNode(vertex);
    root->left->parent = root;
    return root->left;
}

node* insertRight(struct node *root, Number vertex){
    root->right = createNode(vertex);
    root->right->parent = root;
    return root->right;
}

void DFS(Number n, map<Number, int> &visited, Graph &g, set<Number> &vertices) {
    // mark vertex as visited and insert it to vertices(component)
    visited[n] = true;
    vertices.insert(n);

    // insert every not visited naighbor
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
    // mark vertex as visited and insert it to vertices(component)
    visited[n] = true;
    vertices.insert(n);
    // insert every not visited naighbor
    for (auto &inc: graph[n]){
        Number neighbor = inc.n2();
        //neighbor must be not visited yet, not marked as Removed and not in set vertices yet
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

//get components as vectors of Graph
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

// we will get components of graph
set<set<Number>> connectedComponents(Graph &g) {
    set<set<Number>> components;
    map<Number, int> visited;
    for(auto &rot: g) visited[rot.n()]=0;

    // we will find not visited vertex and its neighbors
    for (auto &rot: g) {
        set<Number> vertices;
        if (!visited[rot.n()]) {
            DFS(rot.n(), visited, g, vertices);
            components.insert(vertices);
        }
    }
    return components;
}

//we will get components of graph, vertices marked as removed we ignore
set<set<Number>> connectedComponents(Graph &graph, set<Number> g, map<Number, bool> &isRemoved){
    set<set<Number>> components;
    map<Number, int> visited;
    for(auto &n: g) visited[n]=0;

    // we will find not visited vertex and its neighbors
    for (auto &n: g) {
        if ((!visited[n])&&(!isRemoved[n])) {
            set<Number> vertices;
            DFS2(n, visited, graph, g, vertices, isRemoved);
            components.insert(vertices);
        }
    }
    return components;
}

//not used in my program
//we will get black components of graph g
set<set<Number>> blackComponents(Graph g, multiset<pair<Number, Number>> blackEdges){
    for (auto &rot:g){
        for(auto &inc:g[rot.n()]){
            if ((blackEdges.find(pair(inc.n1(), inc.n2()))==blackEdges.end())&&
            (blackEdges.find(pair(inc.n2(), inc.n1()))==blackEdges.end())){
                deleteE(g, inc.e());
            }
        }
    }
    return connectedComponents(g);
}

// set is positive if it has more internal red edges, negative if there are more external black edges, neutral if equal
inline bool isPositive(set<Number> vertices, Graph &g, multiset<pair<Number, Number>> blackEdges){
    int intRedEdges = 0, extBlackEdges = 0;
    for (auto &n: vertices){
        for (auto &inc: g[n]){
            if (((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
            (blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end()))&&(vertices.find(inc.n2()) == vertices.end()))
                extBlackEdges++;
            else if (((blackEdges.find(pair(inc.n1(), inc.n2()))==blackEdges.end())
            &&(blackEdges.find(pair(inc.n2(), inc.n1()))==blackEdges.end()))&&
            ((vertices.find(inc.n2())!=vertices.end())&&(inc.n1().to_int()<=inc.n2().to_int())))//every edge count only once
                intRedEdges++;
        }
    }
    if (intRedEdges>extBlackEdges)
        return true;
    else return false;
}


inline bool isLarge(set<Number> vertices, float epsilon){
    return (vertices.size() > (1.0+2*epsilon)/epsilon);
}

//optional TODO: opakujuci sa kod vo funkciach alfa() a s() a smallSetsViaRedEdge()

//number of red edges between small components and vertices
int alfa(set<Number> vertices, Graph &g, multiset<pair<Number, Number>> blackEdges,
        float epsilon, set<set<Number>> &connectedComponents){
    int result = 0;
    for (auto &comp: connectedComponents)
        if (!isLarge(vertices, epsilon))
            for (auto &n: vertices)
                for (auto it: g[n].list(IP::primary()))
                    if (((blackEdges.find(pair(it->n1(), it->n2()))==blackEdges.end())&&
                    (blackEdges.find(pair(it->n2(), it->n1()))==blackEdges.end()))&&(comp.find(it->n2()) != comp.end()))
                        result ++;
    return result;
}

//union of vertices and small components to it via red edge
//addOnlyOneSet is true when simple cases such that 2 small sets connected via red edge
set<Number> smallSetsViaRedEdge(set<Number> vertices, Graph &g, multiset<pair<Number, Number>> blackEdges, float epsilon,
                                set<set<Number>> &connectedComponents, bool addOnlyOneSet) {
    set<Number> addedVertices;
    for (auto &n: vertices) addedVertices.insert(n);

    for (auto &comp: connectedComponents){
        bool compFound = false;
        if (!isLarge(vertices, epsilon)) //check only small components
            for (auto &n: comp){
                //for all vertices in this small component if it is connected to set vertices via red edge
                for (auto it: g[n].list(IP::primary())){ //prejdeme hranami daneho vrchola
                    if (((blackEdges.find(pair(it->n1(), it->n2()))==blackEdges.end())&&
                    (blackEdges.find(pair(it->n2(), it->n1()))==blackEdges.end()))
                    &&((vertices.find(it->n2()))!=vertices.end())){
                        for (auto &vertexToAdd: comp)
                            addedVertices.insert(vertexToAdd);
                        compFound = true;
                        break;
                        //after small component is found, increse result, break for cycles and check another small comp.
                    }
                }
                if (compFound) break;
            }
        if (compFound && addOnlyOneSet) //if comp is found and I want only one, return addedComponents
            return addedVertices;
    }
    return addedVertices;
}

//size of union vertices with small components connected to it via red edge
int s(set<Number> &vertices, Graph &g, multiset<pair<Number, Number>> blackEdges,
        float epsilon, set<set<Number>> connectedComponents){
    int result = vertices.size();
    for (auto &comp: connectedComponents){
        bool compFound = false;
        if (!isLarge(vertices, epsilon)) //check only small components
            for (auto &n: vertices){
                //for all vertices in this small component if it is connected to set vertices via red edge
                for (auto it: g[n].list(IP::primary())){
                    if (((blackEdges.find(pair(it->n1(), it->n2()))==blackEdges.end())&&
                    (blackEdges.find(pair(it->n2(), it->n1()))==blackEdges.end()))&&((comp.find(it->n2()))!=comp.end())){
                        result += comp.size();
                        compFound = true;
                        break;
                        //after small component is found, increse result, break for cycles and check another small comp.
                    }
                }
                if (compFound) break;
            }
    }
    return result;
}

bool isThin(set<Number> vertices, Graph &g, multiset<pair<Number, Number>> &blackEdges,
        float epsilon, set<set<Number>> connectedComponents){
    return s(vertices, g, blackEdges, epsilon, connectedComponents)<=
           (alfa(vertices, g, blackEdges, epsilon, connectedComponents)+1)/(epsilon/(1+2*epsilon));

}


//number of edges which can be removed without splitting it into 2 or more components
//tree is connected and has n-1 edges. Therefore, r() = |E| - n + 1
inline int r(set<Number> vertices, Graph g, multiset<pair<Number, Number>> blackEdges){
    int numOfBlackEdges = 0; //number of black edges among vertices
    for (auto &n: vertices){
        for (auto it: g[n].list(IP::primary())){ //check incident edges of vertex n
            if (((blackEdges.find(pair(it->n1(), it->n2()))!=blackEdges.end())||
            (blackEdges.find(pair(it->n2(), it->n1()))!=blackEdges.end()))&&(it->n1().to_int()<it->n2().to_int())){
                numOfBlackEdges++;
            }
        }

    }
    return vertices.size()-numOfBlackEdges+1;


}

/*
* remove small components with cycle of black edges
 * b1 is number of vertices connected to 1 black edges, b3 is number of vertices connected to 3 black edges
*/
void step1(Graph &g, multiset<pair<Number, Number>> blackEdges, float epsilon, set<set<Number>> &connectedComponents,
           map<Number, bool> &isRemoved){
    for (auto &comp: connectedComponents){
        if (!isLarge(comp, epsilon)){
            int b1 = 0, b3 = 0;
            for (auto &n: comp){
                int numOfBlackEdges = 0;
                for(auto &inc: g[n])
                    if((comp.find(inc.n2())!=comp.end())&&((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())
                    ||(blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end())))
                        numOfBlackEdges++;
                assert((numOfBlackEdges>0)&&(numOfBlackEdges<4));
                if (numOfBlackEdges==1)
                    b1++;
                if (numOfBlackEdges==3)
                    b3++;
            }
            if (b1<=b3)
                for (auto &n: comp)
                    isRemoved[n] = true;
        }
    }
}

//we will remove toErase and all small components connected to it via red edge
void eraseLargeThickComp(Graph &g, multiset<pair<Number, Number>> blackEdges, float epsilon,
        set<set<Number>> &connectedComponents, set<Number> toErase, map<Number, bool> &isRemoved){

    for (auto &comp: connectedComponents)
        if (!isLarge(comp, epsilon))
            //for all small components check if it has red edge to toErase, if so, remove this small component
            for (auto &n: comp)
                for (auto it: g[n].list(IP::primary()))
                    if (((blackEdges.find(pair(it->n1(), it->n2())) == blackEdges.end())&&
                    (blackEdges.find(pair(it->n2(), it->n1()))==blackEdges.end()))&&(toErase.find(it->n2())!=toErase.end())
                    &&(!isRemoved[it->n2()]))
                        for (auto &m: comp)
                            isRemoved[m] = true; //removing small component
}

/*
 * remove large thick sets with all adjecent small sets via red edge
 */
void step2(Graph &g, multiset<pair<Number, Number>> blackEdges, float epsilon, set<set<Number>> &connectedComponents,
           map<Number, bool> &isRemoved){
    //we will be removing elements from connectedComponents until there are no such elements.
    //elements can change from thick to thin and vice versa when another element is being removed
    while(true){
        bool breakWhile = false;
        for (auto &comp: connectedComponents){
            // iff we have not removed any element from connectedComponents for whole for loop, we will break
            // while cycle. iff we have removed element, after this for loop we need to do another for loop, because
            // already checked in this elements could change from thick to thin
            if ((isLarge(comp, epsilon))&&(!isThin(comp, g, blackEdges, epsilon, connectedComponents))){
                eraseLargeThickComp(g, blackEdges, epsilon, connectedComponents, comp, isRemoved);
                breakWhile = true;
            }
        }
        if (!breakWhile)
            break;
    }

}

//fat is edge which refers to set vertices
bool isFat(map<pair<Number, Number>, set<Number>> &extraEdges, pair<Number, Number> edge, Graph &g,
        multiset<pair<Number, Number>> &blackEdges, float epsilon, set<set<Number>> connectedComponents){
    set<Number> vertices;
    if (extraEdges.find(edge)!=extraEdges.end()) {
        vertices = extraEdges.at(edge); //because we can obtain non-oriented edge by 2 ways
    } else if (extraEdges.find(pair(edge.second, edge.first))!=extraEdges.end()){
        vertices = extraEdges.at(pair(edge.second, edge.first));
    }
    return s(vertices, g, blackEdges, epsilon, connectedComponents)>
           (4.0/(epsilon/(1.0+2*epsilon)))*alfa(vertices, g, blackEdges, epsilon, connectedComponents);
}

//sum of alfas of non-fat edges (components represented by edge in reduced graph) incident to vertex
int n(Number vertex, map<pair<Number, Number>, set<Number>> &extraEdges, Graph &g,
        multiset<pair<Number, Number>> &blackEdges, float epsilon, set<set<Number>> connectedComponents){
    int totalAlfa = 0;
    for (auto &inc: g[vertex])
        if (extraEdges.find(pair(inc.n1(), inc.n2()))!=extraEdges.end()) {
            if (isFat(extraEdges, pair(inc.n1(), inc.n2()), g, blackEdges, epsilon, connectedComponents))
                totalAlfa += alfa(extraEdges.at(pair(inc.n1(), inc.n2())), g, blackEdges, epsilon, connectedComponents);
        } else
            if (extraEdges.find(pair(inc.n2(), inc.n1()))!=extraEdges.end())
                if (isFat(extraEdges, pair(inc.n2(), inc.n1()), g, blackEdges, epsilon, connectedComponents))
                    totalAlfa += alfa(extraEdges.at(pair(inc.n2(), inc.n1())), g, blackEdges, epsilon, connectedComponents);

    return totalAlfa;
}

//create reference between deleted tree in step 1 a 2 of reducing and the edge which represents it
map<pair<Number, Number>, set<Number>> edgeRef(Graph &graph, map<Number, bool> &isRemoved,
        multiset<pair<Number, Number>> blackEdges){
    map<pair<Number, Number>, set<Number>> er;
    set<set<Number>> components = connectedComponents(graph);
    //int edgeCounter = 0;
    map<Number, bool> isNotRemoved;
    for (int i = 0; i<graph.order(); i++)
        isNotRemoved[i] = !isRemoved[i];

    for (auto &comp: components){
        //kernelOfComponent is set of vertices which were not reduced
        set<Number> kernelOfComponent = connectedComponents(graph, comp, isRemoved).begin().operator*();
        if (!kernelOfComponent.empty()){
            set<set<Number>> reducedTreesOfComponent = connectedComponents(graph, comp, isNotRemoved);
            //TODO: is this working properly?!
            for (auto &rtc: reducedTreesOfComponent){
                vector<Number> vertices;
                //every component will be referenced by 1 edge - from v1 to v2 (v1==v2 is possible)
                bool bothVerticesFound = false;
                for (auto &n: rtc){
                    for (auto &inc: graph[n])
                        if (((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
                             (blackEdges.find(pair(inc.n2(), inc.n1())) != blackEdges.end())) &&
                             (kernelOfComponent.find(inc.n2())!=kernelOfComponent.end())) {
                            vertices.push_back(inc.n2());
                            if (vertices.size()==2) {
                                bothVerticesFound = true;
                                break;
                            }
                        }
                    if (bothVerticesFound) break;
                }
                er.insert(make_pair(make_pair(vertices[0], vertices[1]), rtc));
            }
        }
    }
    return er;
}

void removeLine(Graph &g, set<Number> &graphToReduce, map<Number, bool> &isRemoved,
        const Number n, multiset<pair<Number, Number>> &blackEdges){
    if (!isRemoved[n]){
        int removedNeighbors = 0, numOfBlackEdges = 0;
        for (auto &inc: g[n])
            if ((blackEdges.find(pair(inc.n1(), inc.n2()))!=blackEdges.end())||
                    (blackEdges.find(pair(inc.n2(), inc.n1()))!=blackEdges.end())) {
                numOfBlackEdges++;
                if (isRemoved[inc.n2()])
                    removedNeighbors++;
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

//we will delete all vertices of degree 1 until no such vertex remains
void reduceStep1(Graph &g, set<Number> graphToReduce, map<Number, bool> &isRemoved,
        multiset<pair<Number, Number>> &blackEdges){
    for (auto &n: graphToReduce){
        removeLine(g, graphToReduce, isRemoved, n, blackEdges);
    }
}

//all paths (vertices of degree 2) we delete and replace them by 1 edge which will reference to deleted tree
inline void reduceStep2(Graph &g, set<Number> graphToReduce,  map<Number, bool> &isRemoved,
        multiset<pair<Number, Number>> &blackEdges) {
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
    //we have to mark vertices to delete and after for loop delete them, because iterator would crush and also
    //it would be complicated to check which vertex is of degree 3 and should not be removed
    for (int a = 0; a<g.order(); a++){
        if (toRemove[a]){
            isRemoved[a] = true;
        }
    }
}

node* createSubtree(Graph &graph, node* &root, set<Number> &vertices, multiset<pair<Number, Number>> edges){
    for (auto &inc: graph[root->vertex])
        //because we need to check subtree and through edges included in set "edges"
        if ((vertices.find(inc.n2())!=vertices.end())&&((edges.find(pair(inc.n1(), inc.n2()))!=edges.end())||
                (edges.find(pair(inc.n2(), inc.n1()))!=edges.end()))) {
            if (root->parent->vertex != inc.n2()) { //dont check its parent
                node* newNode = createNode(inc.n2());
                if (root->left == NULL)
                    insertLeft(root, newNode->vertex);
                else insertRight(root, newNode->vertex);
                createSubtree(graph, newNode, vertices, edges);
            }
        }
    return root;
}

set<Number> getSubtreeVertices(node* root, set<Number> &subtreeVertices){
    subtreeVertices.insert(root->vertex);
    if (root->left!=NULL) getSubtreeVertices(root->left, subtreeVertices);
    if (root->right!=NULL) getSubtreeVertices(root->right, subtreeVertices);
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

void designateNodes(Graph &graph, node* root, multiset<pair<Number, Number>> &blackEdges,
        float epsilon, set<set<Number>> components, Number marked1, Number marked2){
    if ((!isNodeInSubtree(graph, root, marked1))&&(!isNodeInSubtree(graph, root, marked2))) {
        set<Number> subtreeVertices = getSubtreeVertices(root, subtreeVertices);
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

void insertParents(set<Number> &graphOfDesignatedVertices, node* currentNode){
    graphOfDesignatedVertices.insert(currentNode->vertex);
    if (currentNode->parent!=NULL)
        insertParents(graphOfDesignatedVertices, currentNode->parent);
}

set<Number> pathsAmongDesignatedVertices(set<node*> &nodes){
    set<Number> result;
    for (auto &n: nodes)
        insertParents(result, n);
    return result;
}

void pathOfChildren(node* currentNode, set<Number> &path){
    if ((currentNode->left!=NULL)&&(currentNode->right==NULL)){
        path.insert(currentNode->vertex);
        pathOfChildren(currentNode->left, path);
    } else if ((currentNode->left==NULL)&&(currentNode->right!=NULL)){
        path.insert(currentNode->vertex);
        pathOfChildren(currentNode->right, path);
    }
}

void getPath(node* currentNode, set<Number> &path, set<Number> &vertices, set<set<Number>> &setOfPaths){
    if (currentNode->parent!=NULL) {
        //I search for nodes with 1 child
        if ((vertices.find(currentNode->parent->left->vertex) == vertices.end()) ||
            (vertices.find(currentNode->parent->right->vertex) == vertices.end())) {
            path.insert(currentNode->parent->vertex);
            getPath(currentNode->parent, path, vertices, setOfPaths);
        } else { //if node has 2 childs, add path to setOfPaths and search for new path
            setOfPaths.insert(path);
            set<Number> newPath;
            getPath(currentNode->parent, newPath, vertices, setOfPaths);
        }
    } else {
        set<Number> newPath;
        newPath.insert(currentNode->vertex);
        pathOfChildren(currentNode, newPath);
        setOfPaths.insert(newPath);
    }
}

set<set<Number>> getAllPaths(set<Number> &vertices, set<node*> &nodes){ //these nodes are designated
    set<set<Number>> setOfPaths;

    for (auto &n: nodes) {
        set<Number> path;
        getPath(n, path, vertices, setOfPaths);
    }

    return setOfPaths;
}

set<node*> getAllChildren(node* root, set<node*> children){
    children.insert(root);
    if (root->left!=NULL) getAllChildren(root->left, children);
    if (root->right!=NULL) getAllChildren(root->right, children);
    return children;
}

set<Number> u(Number vertex, set<node*> &allNodes, set<Number> &graphOfDesignatedVertices){
    set<Number> result;
    for (auto &n: allNodes){
        if (n->vertex!=vertex){
            set<Number> vert;
            //I search only among children because all vertices above designated vertex are also designated
            if (graphOfDesignatedVertices.find(n->left->vertex)==graphOfDesignatedVertices.end()){
                vert = getSubtreeVertices(n->left, vert);
            }
            if (graphOfDesignatedVertices.find(n->right->vertex)==graphOfDesignatedVertices.end()){
                vert = getSubtreeVertices(n->right, vert);
            }
            for (auto &v: vert) result.insert(v);
            result.insert(n->vertex);
        }
    }

    return result;
}

set<Number> u(set<Number> path, set<node*> &allNodes, set<Number> &graphOfDesignatedVertices){
    set<Number> result;
    for (auto &n: allNodes){
        if (path.find(n->vertex)!=path.end()){
            set<Number> vert;
            //I search only among children because all vertices above designated vertex are also designated
            if (graphOfDesignatedVertices.find(n->left->vertex)==graphOfDesignatedVertices.end()){
                vert = getSubtreeVertices(n->left, vert);
            }
            if (graphOfDesignatedVertices.find(n->right->vertex)==graphOfDesignatedVertices.end()){
                vert = getSubtreeVertices(n->right, vert);
            }
            for (auto &v: vert) result.insert(v);
            result.insert(n->vertex);
        }
    }

    return result;
}

bool isPathPlus(set<Number> path, set<node*> &allNodes, set<Number> graphOfDesignatedVertices,
        Graph &blackGraph, multiset<pair<Number, Number>> &blackEdges, float epsilon, set<set<Number>> &connectedComponents){
    set<Number> vertices = u(path, allNodes, graphOfDesignatedVertices);
    return ((4.0/((epsilon/(2*epsilon+1))))*alfa(vertices, blackGraph, blackEdges, epsilon, connectedComponents))>=
    s(vertices, blackGraph, blackEdges, epsilon, connectedComponents);
}

//set of paths "plus" incident to vertex
set<set<Number>> p(Number &vertex, set<set<Number>> &setOfPaths, set<node*> &allNodes,
        set<Number> &graphOfDesignatedVertices, Graph &blackGraph, multiset<pair<Number, Number>> &blackEdges,
        float epsilon, set<set<Number>> &connectedComponents){
    set<set<Number>> result;
    //for all vertex's neighbors check if they are in setOfPaths, if they are and also are "plus", insert them to the result
    if (blackGraph[vertex].neighbours().size()==3){
        for (auto &inc: blackGraph[vertex])
            for (auto &path: setOfPaths)
                if (path.find(inc.n2())!=path.end())
                    if (isPathPlus(path, allNodes, graphOfDesignatedVertices, blackGraph, blackEdges, epsilon, connectedComponents))
                        result.insert(path);
        }
    return result;

}

vector<Number> sortAlfaVertices(Graph &graph, set<Number> path, set<Number> alfaVertices, Number previous,
        Number currentVertex, vector<Number> &sortedAlfaVertices) {
    if (alfaVertices.find(currentVertex)!=alfaVertices.end()){
        sortedAlfaVertices.push_back(currentVertex);
    }
    for (auto &neighbor: graph[currentVertex])
        if ((path.find(neighbor.n2())!=path.end())&&(previous!=neighbor.n2()))
            sortAlfaVertices(graph, path, alfaVertices, currentVertex, neighbor.n2(), sortedAlfaVertices);
    return sortedAlfaVertices;
}

vector<set<Number>> sortAlfaPaths(Graph &graph, set<Number> path, set<Number> alfaVertices, Number previous,
        Number currentVertex, set<Number> &currentPath, vector<set<Number>> &sortedAlfaPaths) {
    if (alfaVertices.find(currentVertex)==alfaVertices.end())
        currentPath.insert(currentVertex);
    else
        sortedAlfaPaths.push_back(currentPath);

    for (auto &neighbor: graph[currentVertex])
        if ((path.find(neighbor.n2())!=path.end())&&(previous!=neighbor.n2()))
            sortedAlfaPaths = sortAlfaPaths(graph, path, alfaVertices, currentVertex, neighbor.n2(),
                                            currentPath, sortedAlfaPaths);
    return sortedAlfaPaths;
}

//function creates reducedGraph from graph - reduceStep1 and reduceStep2
set<Number> reducedGraph(Graph &graph, multiset<pair<Number, Number>> &blackEdges, float epsilon,
        map<Number, bool> &isRemoved, set<set<Number>> blackComponents, Graph &blackGraph, Factory &f){
    for (auto &comp: blackComponents){
        if (isLarge(comp, epsilon)){
            reduceStep1(graph, comp, isRemoved, blackEdges);
            reduceStep2(graph, comp, isRemoved, blackEdges);
        }
    }
    map<pair<Number, Number>, set<Number>> extraEdges = edgeRef(blackGraph, isRemoved, blackEdges);

    Graph rg(createG(f)); //create reduced graph rg
    for (auto &rot: graph) {
        if (!isRemoved[rot.n()])
            addV(rg, rot.n()); // add all remaining (not removed) vertices in graph to rg
    }

    for (auto &rot: rg)
        for (auto &inc: graph[rot.n()])//add black edges between added vertices to rg
            //every vertex check only once, and vertex on the other side of the edge must be not removed
            if ((inc.n1().to_int()<inc.n2().to_int())&&(!isRemoved[inc.n2()])) {
                if (blackEdges.find(pair(inc.n1(), inc.n2())) != blackEdges.end())
                    addE(rg, Loc(inc.n1(), inc.n2()));
                else if (blackEdges.find(pair(inc.n2(), inc.n1())) != blackEdges.end()) //only if edge is not added yet
                    addE(rg, Loc(inc.n2(), inc.n1()));
            }

    for (auto &edge: extraEdges) //add edges between extra verices - these edges reference to removed subgraphs
        addE(rg, rg.find(edge.first.first).operator*().v(), rg.find(edge.first.second).operator*().v());

    set<Number> result;
    for (auto &rot: rg){
        if (n(rot.n(), extraEdges, graph, blackEdges, epsilon, blackComponents)==4){
            result.insert(rot.n());
            for (auto &inc: rot)
                if (!isFat(extraEdges, pair(inc.n1(), inc.n2()), graph, blackEdges, epsilon, blackComponents))
                    for (auto &n:extraEdges.at(pair(inc.n1(), inc.n2())))
                        result.insert(n);

            set<Number> myUnion = smallSetsViaRedEdge(result, graph, blackEdges, epsilon, blackComponents, false);
            for (auto &z: myUnion)
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
                    numOfAlfa2Edges++; //result must be non-fat
                    treesOfAlfa2Edges.insert(vertices);
                }
            }
            if (numOfAlfa2Edges==2) {
                result.insert(rot.n());
                for (auto &tree: treesOfAlfa2Edges)
                    for (auto &n:tree)
                        result.insert(n);
                return result;
            } else {
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
                        //we mark 2 vertices connected via black edges
                        set<Number> k;
                        for (auto &n: vertices)
                            k.insert(n);
                        k.insert(v1);
                        k.insert(v2);

                        struct node* root = createNode(v1);

                        root = createSubtree(graph, root, k, blackEdges);

                        set<node*> allNodes;
                        allNodes = getAllChildren(root, allNodes);

                        designateNodes(graph, root, blackEdges, epsilon, blackComponents, v1, v2);

                        set<node*> nodes;

                        nodes = getDesignatedNodes(root, nodes);

                        for (auto &nd: nodes){
                            set<Number> subtreeVertices = getSubtreeVertices(nd, subtreeVertices);
                            if (s(subtreeVertices, graph, blackEdges, epsilon, blackComponents)<=20/(epsilon/(1+2*epsilon)))
                                return smallSetsViaRedEdge(subtreeVertices, graph, blackEdges, epsilon, blackComponents, false);
                        }

                        set<Number> graphOfDesignatedVertices = pathsAmongDesignatedVertices(nodes);

                        set<set<Number>> allPaths = getAllPaths(graphOfDesignatedVertices, nodes);

                        for (auto n: vertices){
                            set<set<Number>> incidentPaths = p(n, vertices, allPaths, allNodes, graphOfDesignatedVertices,
                                                               blackGraph, blackEdges, epsilon, blackComponents);
                            int sumOfAlpha = 0;
                            for (auto &ip: incidentPaths)
                                sumOfAlpha += alfa(ip, graph, blackEdges, epsilon, blackComponents);

                            if (sumOfAlpha == 4){
                                set<Number> myUnion;
                                for (auto ip: incidentPaths)
                                    for (auto i: u(ip, allNodes, graphOfDesignatedVertices))
                                        myUnion.insert(i);
                                myUnion.insert(n);
                                return smallSetsViaRedEdge(myUnion, graph, blackEdges, epsilon, blackComponents, false);
                            } else { //if (sumOfAlpha>4)
                                set<Number> myUnion;
                                int numOfAlpha2Paths = 0;
                                for (auto ip: incidentPaths) {
                                    if (alfa(ip, graph, blackEdges, epsilon, blackComponents) != 2) {
                                        numOfAlpha2Paths++;
                                        for (auto i: u(ip, allNodes, graphOfDesignatedVertices))
                                            myUnion.insert(i);
                                    }
                                }
                                if (numOfAlpha2Paths==2) {
                                    myUnion.insert(n);
                                    return smallSetsViaRedEdge(myUnion, graph, blackEdges, epsilon, blackComponents, false);
                                } else {
                                    for (auto ip: incidentPaths) {
                                        //we take a look at path with alfa>=3
                                        if (alfa(u(ip, allNodes, graphOfDesignatedVertices), graph, blackEdges, epsilon,
                                                 blackComponents) > 2) {
                                            //distinguish between 2 cases
                                            if (alfa(u(ip, allNodes, graphOfDesignatedVertices), graph, blackEdges,
                                                     epsilon, blackComponents) < 5) {
                                                set<Number> myUnion2;
                                                for (auto &i: ip)
                                                    for (auto &toAdd: u(i, allNodes, graphOfDesignatedVertices))
                                                        myUnion2.insert(toAdd);
                                                return smallSetsViaRedEdge(myUnion2, graph, blackEdges, epsilon,
                                                                           blackComponents, false);
                                            } else {
                                                //find a sequence of 3 vertices on path "ip" such that these vertices' trees are
                                                //connected to small set via red edge. Union of this sequence, paths between them
                                                //and their trees has to be as low as possible

                                                //we need to use vector because we need it sorted as it is connected
                                                set<Number> alfaVertices, currentPath;
                                                set<set<Number>> pathsBetweenAlfaVertices;
                                                vector<Number> sortedAlfaVertices;
                                                vector<set<Number>> sortedAlfaPaths;
                                                Number first, second, third;
                                                set<Number> path;
                                                for (auto &i: ip) {
                                                    if (alfa(u(i, allNodes, graphOfDesignatedVertices), graph,
                                                             blackEdges, epsilon, blackComponents) == 1) {
                                                        alfaVertices.insert(i);
                                                        if (alfaVertices.size() > 1) //we do not add path before 1st vetrtex
                                                            pathsBetweenAlfaVertices.insert(path);
                                                        path.clear();
                                                    } else path.insert(i);
                                                }
                                                for (auto &neighbor: graph[n]){
                                                    if (ip.find(neighbor.n2())!=ip.end()){
                                                        sortedAlfaVertices = sortAlfaVertices(graph, ip, alfaVertices, n, neighbor.n2().to_int(), sortedAlfaVertices);
                                                        sortedAlfaPaths = sortAlfaPaths(graph, ip, alfaVertices, n, neighbor.n2().to_int(), currentPath, sortedAlfaPaths);
                                                    }
                                                }
                                                set<Number> finiteUnion;
                                                int minSizeOfUnion = INT8_MAX;
                                                for (int av = 0; av < alfaVertices.size() - 2; av++) {
                                                    set<Number> myUnion3;
                                                    for (int c = 0; c<3; c++) {
                                                        set<Number> mySet = u(next(alfaVertices.begin(), av + c).operator*(),
                                                                allNodes, graphOfDesignatedVertices);
                                                        for (auto &i: mySet)
                                                            myUnion3.insert(i);
                                                    }
                                                    //c starts on 1 because first path is not between red vertices
                                                    for (int c = 1; c<3; c++) {
                                                        set<Number> mySet =
                                                                u(next(pathsBetweenAlfaVertices.begin(), av + c).operator*(),
                                                                              allNodes, graphOfDesignatedVertices);
                                                        for (auto &i: mySet)
                                                            myUnion3.insert(i);
                                                    }
                                                    if (myUnion3.size() < minSizeOfUnion) {
                                                        minSizeOfUnion = myUnion3.size();
                                                        myUnion3.clear();
                                                        finiteUnion = myUnion3;
                                                    }
                                                }
                                                return finiteUnion;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //just because of warning...
    return result;
}

/*
* lemma 1, see more in pdf file
*/
set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> &blackEdges, float epsilon, Graph &blackGraph, Factory &f) {

    set<set<Number>> blackComponentsFamily = connectedComponents(blackGraph);

    for (auto &rot: graph){
        set<Number> vertex;
        vertex.insert(rot.n());
        if (isPositive(vertex, graph, blackEdges)) return vertex;
    }

    //some simple cases such that small positive set or 2 (or more) small sets connected via a red edge
    for (auto &comp: blackComponentsFamily){
        if (!isLarge(comp, epsilon)) {
            if (isPositive(comp, graph, blackEdges))
                return comp;
            else if (isPositive(smallSetsViaRedEdge(comp, graph, blackEdges, epsilon, blackComponentsFamily, true), graph,
                                blackEdges))
                return smallSetsViaRedEdge(comp, graph, blackEdges, epsilon, blackComponentsFamily, true);
        }
    }

    for (auto &rot: graph)
        for (auto &inc: rot) //we check if there is vertex v such that there exists red edge from v to v
            if ((inc.n2().to_int() == rot.n().to_int())&&
                (blackEdges.find(pair(rot.n().to_int(),rot.n().to_int()))==blackEdges.end())) {
                set<Number> result;
                result.insert(rot.n());
                return result;
            }

    map<Number, bool> isRemoved;
    for (int i = 0; i<graph.order(); i++)
        isRemoved[i] = false;

    step1(graph, blackEdges, epsilon, blackComponentsFamily, isRemoved);

    step2(graph, blackEdges, epsilon, blackComponentsFamily, isRemoved);

    return reducedGraph(graph, blackEdges, epsilon, isRemoved,
            blackComponentsFamily, blackGraph, f);
}

set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> &blackEdges, float epsilon){
    Factory f;
    Graph blackGraph(createG(f));
    for (auto &rot:graph)
        addV(blackGraph, rot.n(), f);
    for (auto &e:blackEdges)
        addE(blackGraph, Loc(e.first, e.second), f);
    return redBlackEdges(graph, blackEdges, epsilon, blackGraph, f);
}

set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> &blackEdges){
    Factory f;
    Graph blackGraph(createG(f));
    for (auto &rot:graph)
        addV(blackGraph, rot.n(), f);
    for (auto &e:blackEdges)
        addE(blackGraph, Loc(e.first, e.second), f);
    //epsilon must be lower than |R|/|V| - 1/2, so therefore it is 2.01
    float epsilon = (graph.size()-blackEdges.size())/float(graph.order())-1/2.01;
    return redBlackEdges(graph, blackEdges, epsilon, blackGraph, f);
}