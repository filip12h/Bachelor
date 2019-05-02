#ifndef BACHELOR_REDBLACKEDGES_H
#define BACHELOR_REDBLACKEDGES_H

#include <set>
#include "impl/basic.hpp"

using namespace std;
using namespace ba_graph;

set<Number> redBlackEdges(Graph &g, set<Edge> &blackEdges, float epsilon);


#endif //BACHELOR_REDBLACKEDGES_H
