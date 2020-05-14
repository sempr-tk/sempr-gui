#ifndef SEMPR_GUI_RULE_HPP_
#define SEMPR_GUI_RULE_HPP_

#include <string>
#include <vector>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace sempr { namespace gui {

/**
    A representation of a rule implemented in the reasoner:
    Its textual representation, its id in the reasoner/sempr, name,
    and a list of nodes (or rather: node ids) in the rete network that
    implement the rules effects.

    Basically a mirror of rete::ParsedRule, just without the actual
    ProductionNode::Ptr.
*/
struct Rule {
    size_t id;
    std::string name;
    std::string ruleString;
    std::vector<std::string> effectNodes;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(id, name, ruleString, effectNodes);
    }
};


}}

#endif /* include guard: SEMPR_GUI_RULE_HPP_ */
