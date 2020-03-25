#ifndef SEMPR_GUI_DIRECTCONNECTION_HPP_
#define SEMPR_GUI_DIRECTCONNECTION_HPP_

#include <sempr/Core.hpp>

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
    callback_t callback_;
    friend class DirectConnectionNode;
public:
    using Ptr = std::shared_ptr<DirectConnection>;
    DirectConnection(sempr::Core* core, std::mutex& m);

    std::vector<ModelEntry> listEntityComponentPairs() override;
    std::string addEntityComponentPair(const ModelEntry&) override;
    void removeEntityComponentPair(const ModelEntry&) override;
    void setUpdateCallback(callback_t) override;

};


}}


#endif /* include guard: SEMPR_GUI_DIRECTCONNECTION_HPP_ */
