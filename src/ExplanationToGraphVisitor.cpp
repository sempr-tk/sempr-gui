#include "ExplanationToGraphVisitor.hpp"
#include <rete-core/Util.hpp>
#include <rete-rdf/ReteRDF.hpp>
#include <rete-reasoner/InferenceState.hpp>

#include <sempr/ECWME.hpp>

namespace sempr { namespace gui {

ExplanationGraph ExplanationToGraphVisitor::graph() const
{
    return graph_;
}

void ExplanationToGraphVisitor::visit(rete::WME::Ptr wme, size_t /*depth*/)
{
    ExplanationNode node;
    node.id = rete::util::ptrToStr(wme.get());
    node.str = wme->toString();

    if (dynamic_cast<rete::Triple*>(wme.get())) node.type = ExplanationNode::Type::TripleWME;
    else if (dynamic_cast<sempr::ECWME*>(wme.get())) node.type = ExplanationNode::Type::ECWME;
    else node.type = ExplanationNode::Type::OtherWME;

    graph_.nodes.insert(node);
}

void ExplanationToGraphVisitor::visit(rete::WMESupportedBy& support, size_t)
{
    for (auto evidence : support.evidences_)
    {
        ExplanationEdge edge(
            rete::util::ptrToStr(evidence.get()),
            rete::util::ptrToStr(support.wme_.get())
        );

        graph_.edges.insert(edge);
    }
}

void ExplanationToGraphVisitor::visit(rete::Evidence::Ptr, size_t)
{
    // do nothing. Only handle Asserted/Inferred evidences explicitely.
}

void ExplanationToGraphVisitor::visit(rete::InferredEvidence::Ptr evidence, size_t)
{
    ExplanationNode node;
    node.id = rete::util::ptrToStr(evidence.get());
    node.str = evidence->toString();
    node.type = ExplanationNode::Type::InferredEvidence;

    graph_.nodes.insert(node);

    // edges to wmes this evidence needs
    rete::Token::Ptr token = evidence->token();
    while (token)
    {
        rete::WME::Ptr wme = token->wme;

        ExplanationEdge edge(
            rete::util::ptrToStr(wme.get()),
            rete::util::ptrToStr(evidence.get())
        );
        graph_.edges.insert(edge);

        token = token->parent;
    }
}

void ExplanationToGraphVisitor::visit(rete::AssertedEvidence::Ptr evidence, size_t)
{
    ExplanationNode node;
    node.id = rete::util::ptrToStr(evidence.get());
    node.str = evidence->toString();
    node.type = ExplanationNode::Type::AssertedEvidence;

    graph_.nodes.insert(node);
}

}}
