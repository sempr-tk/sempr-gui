#include <iostream>
#include <fstream>
#include <thread>

#include <QtCore>

#include "../ui/ui_main.h"
#include "ECModel.hpp"

#include "SemprGui.hpp"

#include <sempr/Core.hpp>
#include <sempr/Entity.hpp>

#include <sempr/nodes/ECNodeBuilder.hpp>
#include <sempr/nodes/ExtractTriplesBuilder.hpp>
#include <sempr/nodes/AffineTransformBuilders.hpp>
#include <sempr/nodes/InferECBuilder.hpp>
#include "DirectConnectionBuilder.hpp"

#include <sempr/component/TripleContainer.hpp>
#include <sempr/component/TripleVector.hpp>
#include <sempr/component/TriplePropertyMap.hpp>
#include <sempr/component/AffineTransform.hpp>
#include <sempr/component/GeosGeometry.hpp>

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/io/WKTReader.h>

#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/RuleParser.hpp>


using namespace sempr;
using namespace sempr::gui;


void runGUI(int argc, char** args, AbstractInterface::Ptr interface)
{
    QApplication app(argc, args);
    qRegisterMetaType<ModelEntry>();
    qRegisterMetaType<ECData>();

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
        [](ECData, AbstractInterface::Notification) -> void
        {
            std::cout << "update callback!" << std::endl;
        }
    );

    rete::RuleParser& parser = sempr.parser();
    parser.registerNodeBuilder<ECNodeBuilder<Component>>();
    parser.registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
    parser.registerNodeBuilder<ECNodeBuilder<TriplePropertyMap>>();
    parser.registerNodeBuilder<ECNodeBuilder<AffineTransform>>();
    parser.registerNodeBuilder<InferECBuilder<AffineTransform>>();
    parser.registerNodeBuilder<AffineTransformCreateBuilder>();
    parser.registerNodeBuilder<DirectConnectionBuilder>(connection);
    parser.registerNodeBuilder<DirectConnectionTripleBuilder>(connection);
    parser.registerNodeBuilder<ExtractTriplesBuilder>();

    auto rules = sempr.addRules(
        // connect to the gui
        "[connectionEC: EC<Component>(?e ?c) -> DirectConnection(?e ?c)]\n" \
        "[connectionTriples: (?s ?p ?o) -> DirectConnectionTriple(?s ?p ?o)]"\
        "[extract: EC<TripleContainer>(?e ?c) -> ExtractTriples(?c)]"
        //"[EC<TripleContainer>(?e ?c), tf:create(?tf 1 2 3 0 0 0 1) -> EC<Transform>(?e ?tf)]" // every entity with a triplecontainer gets a transform
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
    auto vector = std::make_shared<TripleVector>();
    vector->addTriple({"<s1>", "<p1>", "<o1>"});
    vector->addTriple({"<s2>", "<p2>", "<o2>"});
    vector->addTriple({"<s3>", "<p3>", "<o3>"});
    vector->addTriple({"<s4>", "<p4>", "<o4>"});
    entity1->addComponent(vector);

    sempr.performInference();

    {
        std::ofstream("main.dot") << sempr.reasoner().net().toDot();
    }

    // start the gui
    std::thread gui_thread(&runGUI, argc, args, connection);

    // add an entity with a geometry
    auto geoEntity = Entity::create();
    // create a geos geometry
    const geos::geom::GeometryFactory& factory = *geos::geom::GeometryFactory::getDefaultInstance();
    geos::io::WKTReader reader(factory);
    // with 4 coordinates (coord[0] == coord[3])
    std::unique_ptr<geos::geom::Geometry> geosGeometry(reader.read("POLYGON((8.020405035544446 52.26796266283766,8.04975913100343 52.290857976112314,8.07670996718507 52.26554641241761,8.020405035544446 52.26796266283766))"));
    auto geometryComponent = std::make_shared<GeosGeometry>(std::move(geosGeometry));

    std::unique_ptr<geos::geom::Geometry> geosGeometry2(reader.read("POLYGON((8.020405035544446 52.26796266283766,8.04975913100343 52.290857976112314,8.07670996718507 52.26554641241761,8.020405035544446 52.26796266283766))"));
    auto geometryComponent2 = std::make_shared<GeosGeometry>(std::move(geosGeometry2));

    geoEntity->addComponent(geometryComponent);
    geoEntity->addComponent(geometryComponent2);
    sempr.addEntity(geoEntity);

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

