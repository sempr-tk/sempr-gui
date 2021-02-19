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

    // must get exactly three arguments: An entity- and a component-accessor, and a tag
    if (args.size() != 3) throw rete::NodeBuilderException("Wrong number of arguments (!= 3)");

    if(args[0].isConst() || args[1].isConst() || args[2].isConst())
        throw rete::NodeBuilderException("Arguments must not be constants.");

    if (!args[0].getAccessor()->getInterpretation<Entity::Ptr>())
        throw rete::NodeBuilderException("First argument must be an Entity.");
    if (!args[1].getAccessor()->getInterpretation<Component::Ptr>())
        throw rete::NodeBuilderException("Second argument must be a Component.");
    if (!args[2].getAccessor()->getInterpretation<std::string>())
        throw rete::NodeBuilderException("Third argument must be a string.");

    // clone accessors
    rete::PersistentInterpretation<Entity::Ptr> eacc(
        args[0].getAccessor()->getInterpretation<Entity::Ptr>()->makePersistent()
    );

    rete::PersistentInterpretation<Component::Ptr> cacc(
        args[1].getAccessor()->getInterpretation<Component::Ptr>()->makePersistent()
    );

    rete::PersistentInterpretation<std::string> tacc(
        args[2].getAccessor()->getInterpretation<std::string>()->makePersistent()
    );

    // build the node
    auto node = std::make_shared<DirectConnectionNode>(
                    connection_,
                    std::move(eacc),
                    std::move(cacc),
                    std::move(tacc));

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

    rete::PersistentInterpretation<rete::TriplePart> tripleParts[3];
    for (int i = 0; i < 3; i++)
    {
        if (args[i].isConst())
        {
            rete::ConstantAccessor<rete::TriplePart> acc({args[i].getAST()});
            acc.index() = 0;
            tripleParts[i] = acc.getInterpretation<rete::TriplePart>()->makePersistent();
        }
        else
        {
            if (!args[i].getAccessor()->getInterpretation<rete::TriplePart>())
            {
                throw rete::NodeBuilderException("Argument " + args[i].getVariableName() +
                        " does not refer to a part of a triple.");
            }

            tripleParts[i] = args[i].getAccessor()
                                ->getInterpretation<rete::TriplePart>()
                                    ->makePersistent();
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
