#include <iostream>
#include <thread>

#include <QtCore>

#include "../ui/ui_main.h"
#include "ECModel.hpp"

#include "SemprGui.hpp"

#include <sempr/Core.hpp>
#include <sempr/Entity.hpp>

#include <sempr/nodes/ECNodeBuilder.hpp>
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
    parser.registerNodeBuilder<DirectConnectionBuilder>(connection);

    parser.parseRules(
        "[EC<Component>(?e ?c) -> DirectConnection(?e ?c)]",
        sempr.reasoner().net()
    );

    // add an entity to test initialization of the gui
    auto entity = Entity::create();
    auto prop = std::make_shared<TriplePropertyMap>();
    prop->map_["foo"] = "bar";
    prop->map_["baz"] = { "resource", true };
    entity->addComponent(prop);
    sempr.addEntity(entity);
    sempr.performInference();

    // start the gui
    std::thread gui_thread(&runGUI, argc, args, connection);

    // add an entity to test updating the gui
    entity = Entity::create();
    auto affine = std::make_shared<AffineTransform>();
    entity->addComponent(affine);
    sempr.addEntity(entity);


    // ... :/
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        sempr.performInference();
    }


//    ECModel model;
//    ECModel::ModelEntryGroup g1, g2;
//    ModelEntry m11{"Entity_1", "1", {}, true, nullptr};
//    ModelEntry m12{"Entity_1", "2", {}, true, nullptr};
//    g1.push_back(m11);
//    g1.push_back(m12);
//
//    ModelEntry m21{"Entity_2", "3", {}, true, nullptr};
//    ModelEntry m22{"Entity_2", "4", {}, true, nullptr};
//    ModelEntry m23{"Entity_2", "5", {}, true, nullptr};
//    g2.push_back(m21);
//    g2.push_back(m22);
//    g2.push_back(m23);
//
//    model.data_.push_back(g1);
//    model.data_.push_back(g2);


    return 0;
}

