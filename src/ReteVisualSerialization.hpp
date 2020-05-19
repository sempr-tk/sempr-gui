#ifndef SEMPR_GUI_RETEVISUALSERIALIZATION_HPP_
#define SEMPR_GUI_RETEVISUALSERIALIZATION_HPP_

#include <cereal/cereal.hpp>
#include <cereal/types/set.hpp>

#include <rete-core/NodeVisitor.hpp>

#include <string>
#include <set>

/*
    This file provides a few classes to help with creating and transmitting a
    visual representation of a rete network. Some helper structs for the basic
    representation, which can be created through the use of a node visitor
    which traverses the graph. These can be serialized to send them over the
    network.
*/

namespace sempr { namespace gui {

struct Node {
    enum Type { CONDITION, MEMORY, PRODUCTION };
    Type type;
    std::string id;
    std::string label;

    bool operator < (const Node& other) const;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("id", id),
            cereal::make_nvp<Archive>("label", label),
            cereal::make_nvp<Archive>("type", type) );
    }
};

struct Edge {
    std::string from;
    std::string to;

    bool operator < (const Edge& other) const;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("from", from),
            cereal::make_nvp<Archive>("to", to) );
    }
};

struct Graph {
    std::set<Node> nodes;
    std::set<Edge> edges;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("nodes", nodes),
            cereal::make_nvp<Archive>("edges", edges) );
    }
};


class CreateVisualGraphVisitor : public rete::NodeVisitor {
    Graph graph_;

    // a set of already visited nodes, to not visit them twice
    std::set<std::string> visited_;

    void addNode(Node::Type type, const std::string& id, const std::string& label);
    void addEdge(const std::string& from, const std::string& to);
public:
    void visit(rete::AlphaNode*) override;
    void visit(rete::AlphaMemory*) override;
    void visit(rete::BetaNode*) override;
    void visit(rete::BetaMemory*) override;
    void visit(rete::BetaBetaNode*) override;
    void visit(rete::ProductionNode*) override;

    Graph graph() const;
};



}}


#endif /* include guard: SEMPR_GUI_RETEVISUALSERIALIZATION_HPP_ */
