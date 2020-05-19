#ifndef SEMPR_GUI_DIRECTCONNECTIONNODE_HPP_
#define SEMPR_GUI_DIRECTCONNECTIONNODE_HPP_

#include <rete-core/Production.hpp>
#include <sempr/ECAccessors.hpp>

#include "DirectConnection.hpp"

namespace sempr { namespace gui {

/**
    The DirectConnectionNode has access to a DirectConnection and triggers
    its registered callback for entity-component pairs.
*/
class DirectConnectionNode : public rete::Production {
    DirectConnection::Ptr connection_;
    std::unique_ptr<EntityAccessor> entity_;
    std::unique_ptr<ComponentAccessor<Component>> component_;
public:
    using Ptr = std::shared_ptr<DirectConnectionNode>;
    DirectConnectionNode(
        DirectConnection::Ptr connection,
        std::unique_ptr<EntityAccessor> entity,
        std::unique_ptr<ComponentAccessor<Component>> component);

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
    std::unique_ptr<rete::StringAccessor> s_, p_, o_;
public:
    using Ptr = std::shared_ptr<DirectConnectionTripleNode>;

    DirectConnectionTripleNode(
            DirectConnection::Ptr connection,
            std::unique_ptr<rete::StringAccessor> s,
            std::unique_ptr<rete::StringAccessor> p,
            std::unique_ptr<rete::StringAccessor> o);

    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;
    std::string toString() const override;
};

}}

#endif /*influde guard: SEMPR_GUI_DIRECTCONNECTIONNODE_HPP_ */
