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




// -------------------------------------------
// sibling: for triple updates
// -------------------------------------------
DirectConnectionTripleBuilder::DirectConnectionTripleBuilder(DirectConnection::Ptr conn)
    : rete::NodeBuilder("DirectConnectionTriple",
                        rete::NodeBuilder::BuilderType::EFFECT),
      connection_(conn)
{
}


rete::Production::Ptr DirectConnectionTripleBuilder::buildEffect(rete::ArgumentList& args) const
{
    if (!connection_) throw rete::NodeBuilderException("No DirectConnection set in the builder!");

    // must get exactly three arguments: StringAccessors for (?s ?p ?o)
    if (args.size() != 3) throw rete::NodeBuilderException("Wrong number of arguments (!= 3)");

    std::unique_ptr<rete::StringAccessor> tripleParts[3];
    for (int i = 0; i < 3; i++)
    {
        if (args[i].isConst())
        {
            tripleParts[i].reset(new rete::ConstantStringAccessor(args[i].getAST()));
            tripleParts[i]->index() = 0;
        }
        else
        {
            if (!args[i].getAccessor()->canAs<rete::StringAccessor>())
            {
                throw rete::NodeBuilderException("Argument " + args[i].getVariableName() +
                        " is not compatible with StringAccessor");
            }

            tripleParts[i].reset(args[i].getAccessor()->clone()->as<rete::StringAccessor>());
        }
    }

    // build the node
    auto node = std::make_shared<DirectConnectionTripleNode>(
            connection_,
            std::move(tripleParts[0]),
            std::move(tripleParts[1]),
            std::move(tripleParts[2]));

    return node;

}


}}
