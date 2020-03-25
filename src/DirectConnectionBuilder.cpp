#include <rete-reasoner/Exceptions.hpp>

#include "DirectConnectionBuilder.hpp"

namespace sempr { namespace gui {

DirectConnectionBuilder::DirectConnectionBuilder(DirectConnection::Ptr conn)
    : rete::NodeBuilder("DirectConnection",
                        rete::NodeBuilder::BuilderType::EFFECT),
      connection_(conn)
{
}

rete::Production::Ptr DirectConnectionBuilder::buildEffect(rete::ArgumentList& args) const
{
    if (!connection_) throw rete::NodeBuilderException("No DirectConnection set in the builder!");

    // must get exactly two arguments: An entity- and a component-accessor.
    if (args.size() != 2) throw rete::NodeBuilderException("Wrong number of arguments (!= 2)");

    if(args[0].isConst() || args[1].isConst())
        throw rete::NodeBuilderException("Arguments must not be constants.");

    if (!args[0].getAccessor()->canAs<EntityAccessor>())
        throw rete::NodeBuilderException("First argument must be an Entity.");
    if (!args[1].getAccessor()->canAs<ComponentAccessor<Component>>())
        throw rete::NodeBuilderException("Second argument must be a ComponentAccessor.");

    // clone accessors
    std::unique_ptr<EntityAccessor> eacc(args[0].getAccessor()->clone()->as<EntityAccessor>());
    std::unique_ptr<ComponentAccessor<Component>> cacc(args[1].getAccessor()->clone()->as<ComponentAccessor<Component>>());

    // build the node
    auto node = std::make_shared<DirectConnectionNode>(
                    connection_,
                    std::move(eacc),
                    std::move(cacc));

    return node;
}

}}
