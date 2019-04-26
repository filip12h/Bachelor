#ifndef BACHELOR_REDBLACKEDGES_H
#define BACHELOR_REDBLACKEDGES_H

#include <vector>
#include <impl/basic.hpp>

std::vector<ba_graph::Vertex> redBlackEdges(ba_graph::Graph g,std::set<ba_graph::Edge> blackEdges, float epsilon);

#endif //BACHELOR_REDBLACKEDGES_H
