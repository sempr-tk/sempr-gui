#ifndef SEMPR_GUI_EXPLANATIONTOGRAPHVISITOR_HPP_
#define SEMPR_GUI_EXPLANATIONTOGRAPHVISITOR_HPP_

#include <rete-reasoner/ExplanationVisitor.hpp>
#include "ExplanationNode.hpp"

namespace sempr { namespace gui {

class ExplanationToGraphVisitor : public rete::ExplanationVisitor {
    ExplanationGraph graph_;
public:
    void visit(rete::WME::Ptr, size_t depth) override;
    void visit(rete::WMESupportedBy &, size_t depth) override;
    void visit(rete::Evidence::Ptr, size_t depth) override;
    void visit(rete::InferredEvidence::Ptr, size_t depth) override;
    void visit(rete::AssertedEvidence::Ptr, size_t depth) override;

    ExplanationGraph graph() const;
};

}}

#endif /* include guard: SEMPR_GUI_EXPLANATIONTOGRAPHVISITOR_HPP_ */
