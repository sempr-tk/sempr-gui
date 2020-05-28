#ifndef SEMPR_GUI_EXPLANATIONNODE_HPP_
#define SEMPR_GUI_EXPLANATIONNODE_HPP_

#include <string>
#include <set>
#include <utility>

#include <cereal/cereal.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/tuple.hpp>

namespace sempr { namespace gui {

/**
    Class for nodes of a graph representing the explanation of a WME.
*/
struct ExplanationNode {
    enum Type {
        TripleWME = 0,
        ECWME,
        OtherWME,
        InferredEvidence,
        AssertedEvidence
    };

    Type type;

    std::string id;
    std::string str;

    bool operator < (const ExplanationNode& other)
    {
        return id < other.id;
    }

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::make_nvp<Archive>("type", type),
           cereal::make_nvp<Archive>("id", id),
           cereal::make_nvp<Archive>("str", str));
    }
};



typedef std::tuple<std::string, std::string> ExplanationEdge; // <from, to>

struct ExplanationGraph {
    std::set<ExplanationNode> nodes;
    std::set<ExplanationEdge> edges;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::make_nvp<Archive>("nodes", nodes),
           cereal::make_nvp<Archive>("edges", edges));
    }
};

}}

#endif /* include guard: SEMPR_GUI_EXPLANATIONNODE_HPP_ */
