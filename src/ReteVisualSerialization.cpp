#include "ReteVisualSerialization.hpp"

#include <rete-core/AlphaNode.hpp>
#include <rete-core/AlphaMemory.hpp>
#include <rete-core/BetaNode.hpp>
#include <rete-core/BetaMemory.hpp>
#include <rete-core/BetaBetaNode.hpp>
#include <rete-core/ProductionNode.hpp>

namespace sempr { namespace gui {

bool Node::operator<(const Node& other) const
{
    return id < other.id;
}

bool Edge::operator<(const Edge& other) const
{
    if (from == other.from) return to < other.to;
    return from < other.from;
}


void CreateVisualGraphVisitor::addNode(
        const std::string& id,
        const std::string& label)
{
    graph_.nodes.insert({id, label});
}

void CreateVisualGraphVisitor::addEdge(
        const std::string& from,
        const std::string& to)
{
    graph_.edges.insert({from, to});
}

Graph CreateVisualGraphVisitor::graph() const
{
    return graph_;
}

void CreateVisualGraphVisitor::visit(rete::AlphaNode* node)
{
    std::string id = node->getDOTId();

    // stop if already visited
    if (visited_.find(id) != visited_.end()) return;
    visited_.insert(id);

    std::string label = static_cast<rete::Node*>(node)->toString();

    // add this node
    addNode(id, label);

    // add edges to the children and visit them
    std::vector<rete::AlphaNode::Ptr> children;
    node->getChildren(children);

    for (auto child : children)
    {
        if (child)
        {
            addEdge(id, child->getDOTId());
            static_cast<rete::Node*>(child.get())->accept(*this);
        }
    }

    // add edge to alpha memory and visit it
    if (node->getAlphaMemory())
    {
        addEdge(id, node->getAlphaMemory()->getDOTId());
        static_cast<rete::Node*>(node->getAlphaMemory().get())->accept(*this);
    }
}


void CreateVisualGraphVisitor::visit(rete::AlphaMemory* node)
{
    std::string id = node->getDOTId();

    if (visited_.find(id) != visited_.end()) return;
    visited_.insert(id);

    std::string label = static_cast<rete::Node*>(node)->toString();

    // add this node
    addNode(id, label);

    // visit children
    std::vector<rete::BetaNode::Ptr> children;
    node->getChildren(children);

    for (auto child : children)
    {
        if (child)
        {
            addEdge(id, child->getDOTId());
            static_cast<rete::Node*>(child.get())->accept(*this);
        }
    }
}

void CreateVisualGraphVisitor::visit(rete::BetaNode* node)
{
    std::string id = node->getDOTId();

    if (visited_.find(id) != visited_.end()) return;
    visited_.insert(id);

    std::string label = static_cast<rete::Node*>(node)->toString();

    // add this node
    addNode(id, label);

    // visit child node
    if (node->getBetaMemory())
    {
        addEdge(id, node->getBetaMemory()->getDOTId());
        static_cast<rete::Node*>(node->getBetaMemory().get())->accept(*this);
    }
}

void CreateVisualGraphVisitor::visit(rete::BetaMemory* node)
{
    std::string id = node->getDOTId();

    if (visited_.find(id) != visited_.end()) return;
    visited_.insert(id);

    std::string label = static_cast<rete::Node*>(node)->toString();

    // add this node
    addNode(id, label);

    // visit beta node children
    std::vector<rete::BetaNode::Ptr> betaChildren;
    node->getChildren(betaChildren);

    for (auto child : betaChildren)
    {
        if (child)
        {
            addEdge(id, child->getDOTId());
            static_cast<rete::Node*>(child.get())->accept(*this);
        }
    }

    // visit production node children
    std::vector<rete::ProductionNode::Ptr> productionChildren;
    node->getProductions(productionChildren);

    for (auto child : productionChildren)
    {
        if (child)
        {
            addEdge(id, child->getDOTId());
            static_cast<rete::Node*>(child.get())->accept(*this);
        }
    }
}


void CreateVisualGraphVisitor::visit(rete::BetaBetaNode* node)
{
    visit(static_cast<rete::BetaNode*>(node));
}

void CreateVisualGraphVisitor::visit(rete::ProductionNode* node)
{
    std::string id = node->getDOTId();

    if (visited_.find(id) != visited_.end()) return;
    visited_.insert(id);

    std::string label = static_cast<rete::Node*>(node)->toString();

    // add this node
    addNode(id, label);
}


}}
