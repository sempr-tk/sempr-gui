#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include <sempr/ECWME.hpp>
#include <sempr/Component.hpp>
#include <sempr/Entity.hpp>

#include <sstream>
#include <iostream>
#include <typeinfo>
#include <algorithm>

#include "DirectConnection.hpp"

namespace sempr { namespace gui {

DirectConnection::DirectConnection(sempr::Core* core, std::mutex& m)
    : core_(core), semprMutex_(m)
{
}

std::vector<ECData> DirectConnection::listEntityComponentPairs()
{
    std::vector<rete::WME::Ptr> wmes;

    {
        std::lock_guard<std::mutex> lg(core_->reasonerMutex());
        wmes = core_->reasoner().getCurrentState().getWMEs();
    }

    std::vector<ECData> entries;
    for (auto wme : wmes)
    {
        auto ec = std::dynamic_pointer_cast<sempr::ECWME>(wme);
        if (ec)
        {
            auto entity = std::get<0>(ec->value_);
            auto component = std::get<1>(ec->value_);

            ECData entry;
            // populate the model entry with...
            // ... the component "id" (ptr to string)
            entry.componentId = rete::util::ptrToStr(component.get());
            // entity id
            entry.entityId = entity->id();
            // whether it is mutable, i.e., if it is not inferred --
            // we can check this by trying to find the component in the entity.
            // If it is there, it is not inferred.
            auto allComponents = entity->getComponents<Component>();
            bool inferred =
                (std::find(allComponents.begin(), allComponents.end(), component)
                 == allComponents.end());
            entry.isComponentMutable = !inferred;


            // create the serialized version of the component
            std::stringstream ss;
            {
                cereal::JSONOutputArchive ar(ss);
                ar(std::get<1>(ec->value_));
            }
            entry.componentJSON = ss.str();

            // add the entry to the list
            entries.push_back(entry);
        }
    }

    return entries;
}


void DirectConnection::addEntityComponentPair(const ECData& entry)
{
    std::lock_guard<std::mutex> lg(semprMutex_);

    // find the entity
    auto entity = core_->getEntity(entry.entityId);
    if (!entity) throw std::exception(); // TODO: Better exceptions.

    // de-serialize the component
    Component::Ptr c;

    {
        std::stringstream ss(entry.componentJSON);
        cereal::JSONInputArchive ar(ss);
        ar(c);
    } // exceptions?

    entity->addComponent(c);
}


void DirectConnection::removeEntityComponentPair(const ECData& entry)
{
    std::lock_guard<std::mutex> lg(semprMutex_);
    auto entity = core_->getEntity(entry.entityId);
    if (!entity) throw std::exception();

    auto components = entity->getComponents<Component>();
    for (auto c : components)
    {
        if (rete::util::ptrToStr(c.get()) == entry.componentId)
        {
            entity->removeComponent(c);
            return;
        }
    }
}


void DirectConnection::modifyEntityComponentPair(const ECData& entry)
{
    std::lock_guard<std::mutex> lg(semprMutex_);

    // find the entity
    auto entity = core_->getEntity(entry.entityId);
    if (!entity) throw std::exception(); // TODO: Better exceptions.

    // find the component
    auto components = entity->getComponents<Component>();
    for (auto c : components)
    {
        if (rete::util::ptrToStr(c.get()) == entry.componentId)
        {
            // update the component
            // this requires multiple steps:
            // The json we receive is that of a Component::Ptr. To check if
            // it is valid, and matches the type of the actual component, we
            // deserialize it first into a new component.
            std::stringstream ss(entry.componentJSON);
            cereal::JSONInputArchive ar(ss);

            Component::Ptr tmp;
            try {
                ar(tmp);
                // next, check if the type is correct.
                auto& oldC = *c;
                auto& newC = *tmp;
                if (typeid(oldC) == typeid(newC))
                {
                    // types match, all good.
                    // dump a plain version of tmp into a stream
                    std::stringstream ss;
                    {
                        cereal::JSONOutputArchive ar(ss);
                        tmp->saveToJSON(ar);
                    }
                    // and use it to update the existing component
                    cereal::JSONInputArchive ar(ss);
                    c->loadFromJSON(ar);

                    // inform the reasoner
                    c->changed();
                    return;
                }
                // type ids dont match. :(
                throw std::exception();

            } catch (cereal::Exception& e) {
                // ... well.. what to do?
                throw;
            }
        }
    }
    // not found. what to do?
    throw std::exception();
}

}}
