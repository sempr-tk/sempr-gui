#include <cereal/archives/json.hpp>
#include <sstream>

#include "DirectConnectionNode.hpp"

namespace sempr { namespace gui {

DirectConnectionNode::DirectConnectionNode(
        DirectConnection::Ptr conn,
        std::unique_ptr<EntityAccessor> entity,
        std::unique_ptr<ComponentAccessor<Component>> component)
    : connection_(conn),
      entity_(std::move(entity)),
      component_(std::move(component))
{
}

void DirectConnectionNode::execute(
        rete::Token::Ptr token,
        rete::PropagationFlag flag,
        std::vector<rete::WME::Ptr>&)
{
    // extract entity and component from the token
    Entity::Ptr entity;
    Component::Ptr component;
    entity_->SpecificTypeAccessor::getValue(token, entity); // verbose -- forgot a "using SpecificTypeAccessor::getValue" in TokenWMEAccessor.
    component_->getValue(token, component);

    // construct a model entry
    ECData entry;
    entry.componentId = rete::util::ptrToStr(component.get());
    entry.entityId = entity->id();

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar(ss);
        ar(component);
    }
    entry.componentJSON = ss.str();

    // only if the component is also part of the entity (and not only associated
    // in the reasoner) can we modify it.
    entry.isComponentMutable = false;
    for (auto c : entity->getComponents<Component>())
    {
        if (c == component)
        {
            entry.isComponentMutable = true;
            break;
        }
    }

    DirectConnection::Notification n;
    switch (flag) {
        case rete::PropagationFlag::ASSERT:
            n = DirectConnection::ADDED;
            break;
        case rete::PropagationFlag::UPDATE:
            n = DirectConnection::UPDATED;
            break;
        case rete::PropagationFlag::RETRACT:
            n = DirectConnection::REMOVED;
            break;
    }

    // call the callback!
    //if (connection_->callback_) connection_->callback_(entry, n);
    connection_->triggerCallback(entry, n);
}


std::string DirectConnectionNode::toString() const
{
    return "DirectConnection-Update(" + entity_->toString() + ", " + component_->toString() + ")";
}

}}
