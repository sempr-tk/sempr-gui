#include <iostream>
#include <thread>

#include <QtCore>

#include "../ui/ui_main.h"
#include "ECModel.hpp"

#include "SemprGui.hpp"

#include <sempr/Core.hpp>
#include <sempr/Entity.hpp>

#include <sempr/nodes/ECNodeBuilder.hpp>
#include <sempr/nodes/AffineTransformBuilders.hpp>
#include <sempr/nodes/InferECBuilder.hpp>
#include "DirectConnectionBuilder.hpp"

#include <sempr/component/TripleContainer.hpp>
#include <sempr/component/TriplePropertyMap.hpp>
#include <sempr/component/AffineTransform.hpp>

#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/RuleParser.hpp>


using namespace sempr;
using namespace sempr::gui;


void runGUI(int argc, char** args, AbstractInterface::Ptr interface)
{
    QApplication app(argc, args);
    qRegisterMetaType<ModelEntry>();

    SemprGui gui(interface);
    gui.show();

    app.exec();
}


int main(int argc, char** args)
{
    Core sempr;
    std::mutex semprMutex;
    auto connection = std::make_shared<DirectConnection>(&sempr, semprMutex);
    connection->setUpdateCallback(
        [](ModelEntry, AbstractInterface::Notification) -> void
        {
            std::cout << "update callback!" << std::endl;
        }
    );

    rete::RuleParser parser;
    parser.registerNodeBuilder<ECNodeBuilder<Component>>();
    parser.registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
    parser.registerNodeBuilder<ECNodeBuilder<TriplePropertyMap>>();
    parser.registerNodeBuilder<ECNodeBuilder<AffineTransform>>();
    parser.registerNodeBuilder<InferECBuilder<AffineTransform>>();
    parser.registerNodeBuilder<AffineTransformCreateBuilder>();
    parser.registerNodeBuilder<DirectConnectionBuilder>(connection);

    parser.parseRules(
        "[EC<Component>(?e ?c) -> DirectConnection(?e ?c)]\n" // connect to the gui
        "[EC<TripleContainer>(?e ?c), tf:create(?tf 1 2 3 0 0 0 1) -> EC<Transform>(?e ?tf)]", // every entity with a triplecontainer gets a transform
        sempr.reasoner().net()
    );

    // add an entity to test initialization of the gui
    auto entity1 = Entity::create();
    auto prop = std::make_shared<TriplePropertyMap>();
    prop->map_["foo"] = "bar";
    prop->map_["baz"] = { "resource", true };
    prop->map_["some integer"] = 0;
    entity1->addComponent(prop);
    {
        std::lock_guard<std::mutex> lg(semprMutex);
        sempr.addEntity(entity1);
        sempr.performInference();
    }

    // start the gui
    std::thread gui_thread(&runGUI, argc, args, connection);

    // add an entity to test updating the gui
    auto entity = Entity::create();
    auto affine = std::make_shared<AffineTransform>();
    entity->addComponent(affine);

    {
        std::lock_guard<std::mutex> lg(semprMutex);
        sempr.addEntity(entity);
    }


    // just for testing:
    // on every press of "ENTER", increment a value in the property map,
    // and add or remove a component.
    bool add = true;
    while (true)
    {
        // wait for enter key pressed
        while (std::cin.get() != '\n')
        {
            ; // nothing to do
        }

        {
            std::lock_guard<std::mutex> lg(semprMutex);
            // get the property map -- the old one might have been replaced
            // due to an update from the gui
            auto prop = entity1->getComponents<TriplePropertyMap>();
            if (prop.empty())
            {
                throw std::exception(); // WTF?
            }

            // increment value
            int val = prop[0]->map_["some integer"];
            prop[0]->map_["some integer"] = ++val;
            prop[0]->changed();
        }

        // add / remove components
        if (add)
        {
            affine = std::make_shared<AffineTransform>();
            entity->addComponent(affine);
            if (entity->getComponents<Component>().size() >= 10)
            {
                add = false;
            }
        }
        else
        {
            auto components = entity->getComponents<Component>();
            entity->removeComponent(components[0]);
            if (components.size() <= 1)
            {
                add = true;
            }
        }

        // performInference to update the gui
        sempr.performInference();
    }

    return 0;
}

