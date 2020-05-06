#include <sempr/Core.hpp>
#include <sempr/Entity.hpp>

#include <sempr/nodes/ECNodeBuilder.hpp>
#include <sempr/nodes/AffineTransformBuilders.hpp>
#include <sempr/nodes/InferECBuilder.hpp>
#include <sempr/nodes/ExtractTriplesBuilder.hpp>
#include <sempr/nodes/GeoDistanceBuilder.hpp>
#include <sempr/nodes/GeoConversionBuilders.hpp>
#include <sempr/nodes/ConstructRulesBuilder.hpp>
#include <sempr/nodes/TextComponentTextBuilder.hpp>

#include "DirectConnectionBuilder.hpp"
#include "TCPConnectionServer.hpp"

#include <sempr/component/TextComponent.hpp>
#include <sempr/component/TripleContainer.hpp>
#include <sempr/component/TripleVector.hpp>
#include <sempr/component/TriplePropertyMap.hpp>
#include <sempr/component/AffineTransform.hpp>
#include <sempr/component/GeosGeometry.hpp>

#include <sempr/SeparateFileStorage.hpp>

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/io/WKTReader.h>

#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/RuleParser.hpp>

#include <iostream>
#include <fstream>

using namespace sempr;
using namespace sempr::gui;

int main(int argc, char** args)
{
    std::string extraRules;
    if (argc > 1)
    {
        // take first argument as path to rules file
        std::stringstream ss;
        ss << std::ifstream(args[1]).rdbuf();
        extraRules = ss.str();
    }
    else
    {
        // if no rules are explicitely given, add a single rule to allow
        // adding rules like data
        extraRules = "[inferRules: (?a <type> <Rules>), EC<TextComponent>(?a ?c), text:value(?text ?c) -> constructRules(?text)]";
        // actually, we need one more: The basic "Extract Triples" thing to get
        // the "(?a <type> <Rules>)" condition fulfilled.
        extraRules += "[extractTriples: EC<TripleContainer>(?e ?c) -> ExtractTriples(?c)]";
    }

    if (!fs::exists("./db")) fs::create_directory("./db");
    auto db = std::make_shared<SeparateFileStorage>("./db");

    Core sempr(db, db);
    auto savedEntities = db->loadAll();
    for (auto e : savedEntities)
    {
        sempr.addEntity(e);
    }


    std::mutex semprMutex;
    auto connection = std::make_shared<DirectConnection>(&sempr, semprMutex);

    // just creating it already serves updates, just not the requests yet
    TCPConnectionServer server(connection);
    server.start();

    // but we still need to insert the connection into the reasoner
    rete::RuleParser parser;
    parser.registerNodeBuilder<ECNodeBuilder<Component>>();
    parser.registerNodeBuilder<ECNodeBuilder<TextComponent>>();
    parser.registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
    parser.registerNodeBuilder<ECNodeBuilder<TriplePropertyMap>>();
    parser.registerNodeBuilder<ECNodeBuilder<AffineTransform>>();
    parser.registerNodeBuilder<ECNodeBuilder<GeosGeometry>>();
    parser.registerNodeBuilder<ECNodeBuilder<GeosGeometryInterface>>();
    parser.registerNodeBuilder<TextComponentTextBuilder>();
    parser.registerNodeBuilder<ConstructRulesBuilder>(&sempr);
    parser.registerNodeBuilder<InferECBuilder<AffineTransform>>();
    parser.registerNodeBuilder<AffineTransformCreateBuilder>();
    parser.registerNodeBuilder<DirectConnectionBuilder>(connection);
    parser.registerNodeBuilder<ExtractTriplesBuilder>();
    parser.registerNodeBuilder<GeoDistanceBuilder>();
    parser.registerNodeBuilder<UTMFromWGSBuilder>();

    auto rules = parser.parseRules(
        "[EC<Component>(?e ?c) -> DirectConnection(?e ?c)]\n", // connect to the gui
        sempr.reasoner().net()
    );

    if (!extraRules.empty())
    {
        auto moreRules = parser.parseRules(extraRules, sempr.reasoner().net());
        rules.insert(rules.end(), moreRules.begin(), moreRules.end());
    }


    sempr.performInference();
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
