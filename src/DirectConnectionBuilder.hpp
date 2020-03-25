#ifndef SEMPR_GUI_DIRECTCONNECTIONBUILDER_HPP_
#define SEMPR_GUI_DIRECTCONNECTIONBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include "DirectConnectionNode.hpp"

namespace sempr { namespace gui {

/**
    Constructs the DirectConnectionNodes.
    Takes a DirectConnection::Ptr that will be used to trigger a callback
    function registered there.
*/
class DirectConnectionBuilder : public rete::NodeBuilder {
    DirectConnection::Ptr connection_;
public:
    DirectConnectionBuilder(DirectConnection::Ptr);
    rete::Production::Ptr buildEffect(rete::ArgumentList& args) const override;
};

}}

#endif /* include guard: SEMPR_GUI_DIRECTCONNECTIONBUILDER_HPP_ */
