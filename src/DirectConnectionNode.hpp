#ifndef SEMPR_GUI_DIRECTCONNECTIONNODE_HPP_
#define SEMPR_GUI_DIRECTCONNECTIONNODE_HPP_

#include <rete-core/Production.hpp>
#include <rete-rdf/TriplePart.hpp>
#include <sempr/ECAccessors.hpp>

#include "DirectConnection.hpp"

namespace sempr { namespace gui {

/**
    The DirectConnectionNode has access to a DirectConnection and triggers
    its registered callback for entity-component pairs.
*/
class DirectConnectionNode : public rete::Production {
    DirectConnection::Ptr connection_;
    rete::PersistentInterpretation<Entity::Ptr> entity_;
    rete::PersistentInterpretation<Component::Ptr> component_;
    rete::PersistentInterpretation<std::string> tag_;
public:
    using Ptr = std::shared_ptr<DirectConnectionNode>;
    DirectConnectionNode(
        DirectConnection::Ptr connection,
        rete::PersistentInterpretation<Entity::Ptr> entity,
        rete::PersistentInterpretation<Component::Ptr> component,
        rete::PersistentInterpretation<std::string> tag);

    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;

    std::string toString() const override;
};


/**
    The DirectConnectionTripleNode is a sibling of the DirectConnectionNode.
    It triggers a callback in the same manner as the latter, but for
    to transmit triples instead of EC pairs.
*/
class DirectConnectionTripleNode : public rete::Production {
    DirectConnection::Ptr connection_;
    rete::PersistentInterpretation<rete::TriplePart> s_, p_, o_;
public:
    using Ptr = std::shared_ptr<DirectConnectionTripleNode>;

    DirectConnectionTripleNode(
            DirectConnection::Ptr connection,
            rete::PersistentInterpretation<rete::TriplePart> s,
            rete::PersistentInterpretation<rete::TriplePart> p,
            rete::PersistentInterpretation<rete::TriplePart> o);

    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;
    std::string toString() const override;
};

}}

#endif /*influde guard: SEMPR_GUI_DIRECTCONNECTIONNODE_HPP_ */
