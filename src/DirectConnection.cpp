#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include <sempr/ECWME.hpp>
#include <sempr/Component.hpp>
#include <sempr/Entity.hpp>

#include <sstream>
#include <iostream>

#include "DirectConnection.hpp"

namespace sempr { namespace gui {

DirectConnection::DirectConnection(sempr::Core* core, std::mutex& m)
    : core_(core), semprMutex_(m)
{
}

std::vector<ModelEntry> DirectConnection::listEntityComponentPairs()
{
    std::vector<rete::WME::Ptr> wmes;

    {
        std::lock_guard<std::mutex> lg(core_->reasonerMutex());
        wmes = core_->reasoner().getCurrentState().getWMEs();
    }

    std::vector<ModelEntry> entries;
    for (auto wme : wmes)
    {
        auto ec = std::dynamic_pointer_cast<sempr::ECWME>(wme);
        if (ec)
        {
            auto entity = std::get<0>(ec->value_);
            auto component = std::get<1>(ec->value_);

            ModelEntry entry;
            // populate the model entry with...
            // ... the component "id" (ptr to string)
            entry.componentId_ = rete::util::ptrToStr(component.get());
            // entity id
            entry.entityId_ = entity->id();
            // whether it is mutable, i.e., if it is not inferred --
            // we can check this by trying to find the component in the entity.
            // If it is there, it is not inferred.
            auto allComponents = entity->getComponents<Component>();
            bool inferred =
                (std::find(allComponents.begin(), allComponents.end(), component)
                 == allComponents.end());
            entry.mutable_ = !inferred;


            // create the serialized version of the component
            std::stringstream ss;
            {
                cereal::JSONOutputArchive ar(ss);
                ar(std::get<1>(ec->value_));
            }
            entry.componentJSON_ = ss.str();

            // and try to deserialize it to effectively create a copy.
            // (why try? must work, else serialization just before this would
            // have failed alreay... just remember to *try* deserialization of
            // the json representation in different implementations!)
            try {
                cereal::JSONInputArchive ar(ss);
                ar(entry.component_);
            } catch (cereal::Exception& e) {
                std::cerr << e.what() << std::endl;
                entry.component_ = nullptr;
            }

            // add the entry to the list
            entries.push_back(entry);
        }
    }

    return entries;
}


std::string DirectConnection::addEntityComponentPair(const ModelEntry& entry)
{
    std::lock_guard<std::mutex> lg(semprMutex_);

    // find the entity
    auto entity = core_->getEntity(entry.entityId_);
    if (!entity) throw std::exception(); // TODO: Better exceptions.

    // de-serialize the component
    Component::Ptr c;

    {
        std::stringstream ss(entry.componentJSON_);
        cereal::JSONInputArchive ar(ss);
        ar(c);
    } // exceptions?

    entity->addComponent(c);

    return rete::util::ptrToStr(c.get());
}


void DirectConnection::removeEntityComponentPair(const ModelEntry& entry)
{
    std::lock_guard<std::mutex> lg(semprMutex_);
    auto entity = core_->getEntity(entry.entityId_);
    if (!entity) throw std::exception();

    auto components = entity->getComponents<Component>();
    for (auto c : components)
    {
        if (rete::util::ptrToStr(c.get()) == entry.componentId_)
        {
            entity->removeComponent(c);
            return;
        }
    }
}


void DirectConnection::setUpdateCallback(callback_t cb)
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    callback_ = cb;
}

void DirectConnection::clearUpdateCallback()
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    callback_ = nullptr;
}

void DirectConnection::triggerCallback(
        callback_t::first_argument_type arg1,
        callback_t::second_argument_type arg2)
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    callback_(arg1, arg2);
}

}}
