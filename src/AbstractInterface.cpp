#include "AbstractInterface.hpp"


namespace sempr { namespace gui {


void AbstractInterface::setUpdateCallback(callback_t cb)
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    callback_ = cb;
}

void AbstractInterface::clearUpdateCallback()
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    callback_ = nullptr;
}

void AbstractInterface::triggerCallback(
        callback_t::first_argument_type arg1,
        callback_t::second_argument_type arg2)
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    if (callback_) callback_(arg1, arg2);
}

}}
