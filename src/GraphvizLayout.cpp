#include "GraphvizLayout.hpp"

#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#include <QString>
#include <map>
#include <rete-core/Util.hpp> // ptr to string
#include <iostream>

namespace sempr { namespace gui {


// helper functions... the api of graphviz is garbage! "char*" everywhere, but
// actually the only need "const char*" ...
// -> http://www.mupuf.org/blog/2010/07/08/how_to_use_graphviz_to_draw_graphs_in_a_qt_graphics_scene/

Agraph_t* _agopen(const char* name, Agdesc_t desc, Agdisc_t* disc)
{
    return agopen(const_cast<char*>(name), desc, disc);
}

Agnode_t* _agnode(Agraph_t* graph, const char* name, int createflag)
{
    return agnode(graph, const_cast<char*>(name), createflag);
}

Agedge_t* _agedge(Agraph_t* graph, Agnode_t* t, Agnode_t* h, const char* name, int createflag)
{
    return agedge(graph, t, h, const_cast<char*>(name), createflag);
}

Agsym_t* _agattr(Agraph_t* graph, int kind, const char* name, const char* value)
{
    return agattr(graph, kind, const_cast<char*>(name), const_cast<char*>(value));
}

int _agset(void* obj, const char* name, const char* value)
{
    return agset(obj, const_cast<char*>(name), const_cast<char*>(value));
}


void GraphvizLayout::layout(const std::vector<GraphNodeItem*>& nodes,
                            const std::vector<GraphEdgeItem*>& edges)
{
    // create the graphviz context and a graph
    GVC_t* gvc = gvContext();
    Agraph_t* G = _agopen("mygraph", Agdirected, nullptr);

    // create default attributes (must be done before assigning attributes to
    // nodes/edges

    // Use a fixed size for nodes, independent of their label + font. We will
    // set the size based on the boundingRect() of the graphics items.
    _agattr(G, AGNODE, "fixedsize", "shape");
    _agattr(G, AGRAPH, "overlap", "prism"); // remove node overlaps (doesnt change anything?)
    _agattr(G, AGRAPH, "pad", "0.2"); // inches padding for nodes (doesnt change anything?)
    _agattr(G, AGRAPH, "nodesep", "0.4"); // inches separation between nodes (made things worse)

    // set the dpi to use -- sizes will be given in inches, coordinates in dots
    const float dpi = 96.0f;
    _agattr(G, AGRAPH, "dpi", "96.0");
    // To be able to set a width and height, we need a default width and height
    // first. These values are in inches.
    _agattr(G, AGNODE, "width", "1");
    _agattr(G, AGNODE, "height", "1");
    // set a default label -- just an empty string.
    _agattr(G, AGNODE, "label", "");

    // create all nodes
    std::map<GraphNodeItem*, Agnode_t*> agNodes;
    for (auto& node : nodes)
    {
        auto rect = node->boundingRect();

        QString width = QString::number(rect.width() / dpi);
        QString height = QString::number(rect.height() / dpi);

        std::string nodeId = rete::util::ptrToStr(node);

        Agnode_t* newNode = _agnode(G, nodeId.c_str(), true);
        _agset(newNode, "width", width.toStdString().c_str());
        _agset(newNode, "height", height.toStdString().c_str());

        agNodes[node] = newNode;
    }

    // create all edges
    for (auto& edge : edges)
    {
        auto fromNode = agNodes[edge->from()];
        auto toNode = agNodes[edge->to()];

        _agedge(G, fromNode, toNode, nullptr, true);
    }

    // do the layout
    gvLayout(gvc, G, "dot");

    // dot uses 72 dpi, so we need to scale a bit.
    // Actually... I'm not sure. Why did we then set the 96 dpi setting in the
    // beginning?!
    const float DotDefaultDPI = 72.f;
    float scale = dpi / DotDefaultDPI;

    // apply layout information to GraphNodeItems
    for (auto& entry : agNodes)
    {
        auto item = entry.first;
        auto agNode = entry.second;

        auto coord = ND_coord(agNode);
        item->setPos(coord.x * scale, coord.y * scale);
        item->update();
    }

    // cleanup
    gvFreeLayout(gvc, G);
    agclose(G);
}


}}
