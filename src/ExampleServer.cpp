#include <sempr/Core.hpp>
#include <sempr/Entity.hpp>

#include <sempr/nodes/ECNodeBuilder.hpp>
#include <sempr/nodes/AffineTransformBuilders.hpp>
#include <sempr/nodes/InferECBuilder.hpp>
#include "DirectConnectionBuilder.hpp"
#include "TCPConnectionServer.hpp"

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

#include <iostream>
#include <fstream>

using namespace sempr;
using namespace sempr::gui;



int main()
{
    Core sempr;
    std::mutex semprMutex;
    auto connection = std::make_shared<DirectConnection>(&sempr, semprMutex);

    // just creating it already serves updates, just not the requests yet
    TCPConnectionServer server(connection);
    server.start();

    // but we still need to insert the connection into the reasoner
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

    // add some data
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

    // add an entity with a geometry
    auto geoEntity = Entity::create();
    // create a geos geometry
    const geos::geom::GeometryFactory& factory = *geos::geom::GeometryFactory::getDefaultInstance();
    geos::io::WKTReader reader(factory);
    // with 4 coordinates (coord[0] == coord[3])
    auto geosGeometry = reader.read("POLYGON((8.020405035544446 52.26796266283766,8.04975913100343 52.290857976112314,8.07670996718507 52.26554641241761,8.020405035544446 52.26796266283766))");
    auto geometryComponent = std::make_shared<GeosGeometry>(geosGeometry);

    auto geosGeometry2 = reader.read("POLYGON((8.120405035544446 52.26796266283766,8.14975913100343 52.290857976112314,8.17670996718507 52.26554641241761,8.120405035544446 52.26796266283766))");
    auto geometryComponent2 = std::make_shared<GeosGeometry>(geosGeometry2);

    geoEntity->addComponent(geometryComponent);
    geoEntity->addComponent(geometryComponent2);
    sempr.addEntity(geoEntity);

    {
        std::ofstream("debug.dot") << sempr.reasoner().net().toDot();
    }

    std::cout << "starting reasoning-loop" << std::endl;
    while (true)
    {
        sempr.performInference();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // there should probably be some kind of signalling system instead of
        // this kind of polling...
    }
}
