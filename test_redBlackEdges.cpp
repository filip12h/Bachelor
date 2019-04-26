#include <cassert>
#include <basic_impl.hpp>
#include <graphs.hpp>
#include <io/graph6.hpp>
#include <operations/basic.hpp>
#include "redBlackEdges.h"

using namespace ba_graph;

int main(){
    Factory f;
    Graph g(createG(f));

    for(int i=0;i<8;i++) addV(g, i, f);
    for(int i=0;i<3;i++) addE(g, Loc(i, i+1), f);
    addE(g, Loc(3,0), f);
    for(int i=4;i<6;i++) addE(g, Loc(i, i+1), f);

    std::cout<<g;



    return 0;
}