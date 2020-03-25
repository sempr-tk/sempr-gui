#include <cereal/archives/json.hpp>
#include <sstream>

#include "DirectConnectionNode.hpp"
#include "ModelEntry.hpp"

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
    ModelEntry entry;
    entry.componentId_ = rete::util::ptrToStr(component.get());
    entry.entityId_ = entity->id();

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar(ss);
        ar(component);
    }
    entry.componentJSON_ = ss.str();
    cereal::JSONInputArchive ar(ss);
    ar(entry.component_); // effectively create a copy of the component

    // only if the component is also part of the entity (and not only associated
    // in the reasoner) can we modify it.
    entry.mutable_ = false;
    for (auto c : entity->getComponents<Component>())
    {
        if (c == component)
        {
            entry.mutable_ = true;
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
    connection_->callback_(entry, n);
}


std::string DirectConnectionNode::toString() const
{
    return "DirectConnection-Update(" + entity_->toString() + ", " + component_->toString() + ")";
}

}}
