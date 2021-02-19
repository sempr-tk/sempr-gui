#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include <sempr/ECWME.hpp>
#include <sempr/Component.hpp>
#include <sempr/Entity.hpp>
#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/InferenceState.hpp>

#include <sstream>
#include <iostream>
#include <typeinfo>
#include <algorithm>

#include "DirectConnection.hpp"
#include "ExplanationToGraphVisitor.hpp"

namespace sempr { namespace gui {

DirectConnection::DirectConnection(sempr::Core* core, std::mutex& m)
    : core_(core), semprMutex_(m)
{
}


Graph DirectConnection::getReteNetworkRepresentation()
{
    std::lock_guard<std::recursive_mutex> lg(core_->reasonerMutex());

    CreateVisualGraphVisitor visitor;
    static_cast<rete::Node*>(core_->reasoner().net().getRoot().get())->accept(visitor);

    return visitor.graph();
}

ExplanationGraph DirectConnection::getExplanationGeneric(rete::WME::Ptr wme)
{
    ExplanationToGraphVisitor visitor;

    {
        std::lock_guard<std::recursive_mutex> lg(core_->reasonerMutex());
        auto infstate = core_->reasoner().getCurrentState();

        infstate.traverseExplanation(wme, visitor);
    }

    return visitor.graph();
}


ExplanationGraph DirectConnection::getExplanation(const ECData& ec)
{
    std::lock_guard<std::recursive_mutex> lg(core_->reasonerMutex());
    auto infstate = core_->reasoner().getCurrentState();
    auto wmes = infstate.getWMEs();
    auto toExplain = std::find_if(wmes.begin(), wmes.end(),
            [&ec](rete::WME::Ptr wme) -> bool
            {
                auto ecwme = std::dynamic_pointer_cast<ECWME>(wme);
                if (ecwme)
                {
                    return std::get<0>(ecwme->value_)->id() == ec.entityId &&
                           rete::util::ptrToStr(std::get<1>(ecwme->value_).get()) == ec.componentId;
                }
                return false;
            });

    if (toExplain == wmes.end()) return ExplanationGraph();

    return getExplanationGeneric(*toExplain);
}


ExplanationGraph DirectConnection::getExplanation(sempr::Triple::Ptr triple)
{
    auto wme = std::make_shared<rete::Triple>(
        triple->getField(sempr::Triple::Field::SUBJECT),
        triple->getField(sempr::Triple::Field::PREDICATE),
        triple->getField(sempr::Triple::Field::OBJECT)
    );

    return getExplanationGeneric(wme);
}


std::vector<Rule> DirectConnection::getRulesRepresentation()
{
    std::vector<rete::ParsedRule::Ptr> reteRules;
    {
        std::lock_guard<std::mutex> lg(semprMutex_);
        reteRules = core_->rules();
    }

    std::vector<Rule> rules;
    for (auto r : reteRules)
    {
        Rule rule;
        rule.id = r->id();
        rule.name = r->name();
        rule.ruleString = r->ruleString();

        for (auto effect : r->effectNodes())
        {
            rule.effectNodes.push_back(effect->getDOTId());
        }

        rules.push_back(rule);
    }

    return rules;
}

std::vector<sempr::Triple> DirectConnection::listTriples()
{
    std::vector<sempr::Triple> triples;
    std::vector<rete::WME::Ptr> wmes;

    {
        std::lock_guard<std::recursive_mutex> lg(core_->reasonerMutex());
        wmes = core_->reasoner().getCurrentState().getWMEs();
    }

    for (auto wme : wmes)
    {
        auto triple = std::dynamic_pointer_cast<rete::Triple>(wme);
        if (triple)
        {
            sempr::Triple st(triple->subject,
                             triple->predicate,
                             triple->object);

            triples.push_back(st);
        }
    }

    return triples;
}

std::vector<ECData> DirectConnection::listEntityComponentPairs()
{
    std::vector<rete::WME::Ptr> wmes;

    {
        std::lock_guard<std::recursive_mutex> lg(core_->reasonerMutex());
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
            auto tag = std::get<2>(ec->value_);

            ECData entry;
            // populate the model entry with...
            // ... the component "id" (ptr to string)
            entry.componentId = rete::util::ptrToStr(component.get());
            // entity id
            entry.entityId = entity->id();
            entry.tag = tag;

            // whether it is mutable, i.e., if it is not inferred --
            // we can check this by trying to find the component in the entity.
            // If it is there, it is not inferred.
            auto allComponents = entity->getComponentsWithTag<Component>();
            std::pair<Component::Ptr, std::string> searchFor(component, tag);
            bool inferred =
                (std::find(allComponents.begin(), allComponents.end(), searchFor)
                 == allComponents.end());
            entry.isComponentMutable = !inferred;

            std::cout << entity->id() << " " << component.get() << " isComponentMutable ? " << entry.isComponentMutable << std::endl;


            // create the serialized version of the component
            std::stringstream ss;
            {
                cereal::JSONOutputArchive ar(ss);
                ar(std::get<1>(ec->value_));
            }
            entry.componentJSON = ss.str();
            entry.tag = tag;

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

    // create it if it does not exist yet
    if (!entity)
    {
        entity = Entity::create();
        // if the given id was not empty, use that
        if (!entry.entityId.empty()) entity->setId(entry.entityId);
        // and add it to the core
        core_->addEntity(entity);
    }

    // de-serialize the component
    Component::Ptr c;

    {
        std::stringstream ss(entry.componentJSON);
        cereal::JSONInputArchive ar(ss);
        ar(c);
    } // exceptions?

    entity->addComponent(c, entry.tag);
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
    auto components = entity->getComponentsWithTag<Component>();
    for (auto ct : components)
    {
        if (rete::util::ptrToStr(std::get<0>(ct).get()) == entry.componentId)
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
                auto& oldC = *std::get<0>(ct);
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
                    std::get<0>(ct)->loadFromJSON(ar);

                    if (entry.tag == std::get<1>(ct))
                    {
                        // the tag has not changed, it suffices to call "changed"
                        std::get<0>(ct)->changed();
                    }
                    else
                    {
                        // the tag changed, we need to remove and re-add the
                        // component from its entity
                        entity->removeComponent(std::get<0>(ct));
                        entity->addComponent(std::get<0>(ct), entry.tag);
                    }
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
