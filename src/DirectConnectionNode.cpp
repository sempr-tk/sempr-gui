#include <cereal/archives/json.hpp>
#include <sstream>
#include <sempr/component/TripleContainer.hpp>

#include "DirectConnectionNode.hpp"

namespace sempr { namespace gui {

DirectConnectionNode::DirectConnectionNode(
        DirectConnection::Ptr conn,
        rete::PersistentInterpretation<Entity::Ptr> entity,
        rete::PersistentInterpretation<Component::Ptr> component)
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
    entity_.interpretation->getValue(token, entity);
    component_.interpretation->getValue(token, component);

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
    return "DirectConnection-Update(" +
            entity_.accessor->toString() + ", " +
            component_.accessor->toString() + ")";
}


// -------------------------------------------
// sibling: DirectConnection*TRIPLE*Node
// -------------------------------------------
DirectConnectionTripleNode::DirectConnectionTripleNode(
            DirectConnection::Ptr con,
            rete::PersistentInterpretation<rete::TriplePart> s,
            rete::PersistentInterpretation<rete::TriplePart> p,
            rete::PersistentInterpretation<rete::TriplePart> o)
    : connection_(con),
      s_(std::move(s)),
      p_(std::move(p)),
      o_(std::move(o))
{
}

void DirectConnectionTripleNode::execute(
        rete::Token::Ptr token,
        rete::PropagationFlag flag,
        std::vector<rete::WME::Ptr>&)
{
    DirectConnection::Notification n;
    if (flag == rete::PropagationFlag::ASSERT) n = DirectConnection::ADDED;
    else if (flag == rete::PropagationFlag::UPDATE) n = DirectConnection::UPDATED;
    else /*if (n == rete::PropagationFlag::RETRACT)*/ n = DirectConnection::REMOVED;

    rete::TriplePart s, p, o;
    s_.interpretation->getValue(token, s);
    p_.interpretation->getValue(token, p);
    o_.interpretation->getValue(token, o);

    sempr::Triple triple(s.value, p.value, o.value);

    // trigger the callback
    connection_->triggerTripleCallback(triple, n);
}


std::string DirectConnectionTripleNode::toString() const
{
    return "DirectConnection-TripleUpdate\n" +
            s_.accessor->toString() + ", " +
            p_.accessor->toString() + ", " +
            o_.accessor->toString();
}

}}
