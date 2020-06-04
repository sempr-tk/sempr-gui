#ifndef SEMPR_GUI_GRAPHVIZLAYOUT_HPP_
#define SEMPR_GUI_GRAPHVIZLAYOUT_HPP_

#include <string>
#include <map>
#include <vector>
#include <utility>

#include "GraphNodeItem.hpp"
#include "GraphEdgeItem.hpp"

namespace sempr { namespace gui {

/**
    Utility to compute the layout of QGraphicItems in a scene, given a list
    of edges between them.
*/
class GraphvizLayout {
    GraphvizLayout() = delete;

public:
    /**
        Takes lists of nodes and edges and adjusts their layout by using the
        graphviz library.
    */
    static void layout(const std::vector<GraphNodeItem*>& nodes,
                       const std::vector<GraphEdgeItem*>& edges);
};

}}

#endif /* include guard: SEMPR_GUI_GRAPHVIZLAYOUT_HPP_ */
