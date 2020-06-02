#ifndef SEMPR_GUI_DIRECTCONNECTION_HPP_
#define SEMPR_GUI_DIRECTCONNECTION_HPP_

#include <sempr/Core.hpp>
#include <mutex>

#include "AbstractInterface.hpp"

namespace sempr { namespace gui {

/**
    The DirectConnection owns a pointer to the sempr instance we want to
    connect to. Still, to help with threading issues, it copies components by
    serializing + deserializing them, instead of just handing it the pointer.
*/
class DirectConnection : public AbstractInterface {
    sempr::Core* core_;
    std::mutex& semprMutex_;

protected:
    ExplanationGraph getExplanationGeneric(rete::WME::Ptr wme);

public:
    using Ptr = std::shared_ptr<DirectConnection>;
    DirectConnection(sempr::Core* core, std::mutex& m);

    Graph getReteNetworkRepresentation() override;
    ExplanationGraph getExplanation(const ECData &ec) override;
    ExplanationGraph getExplanation(sempr::Triple::Ptr triple) override;
    std::vector<Rule> getRulesRepresentation() override;
    std::vector<ECData> listEntityComponentPairs() override;
    std::vector<sempr::Triple> listTriples() override;
    void addEntityComponentPair(const ECData&) override;
    void removeEntityComponentPair(const ECData&) override;
    void modifyEntityComponentPair(const ECData&) override;
};


}}


#endif /* include guard: SEMPR_GUI_DIRECTCONNECTION_HPP_ */
