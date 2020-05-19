#include "AbstractInterface.hpp"


namespace sempr { namespace gui {


void AbstractInterface::setUpdateCallback(callback_t cb)
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    callback_ = cb;
}

void AbstractInterface::setTripleUpdateCallback(triple_callback_t cb)
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    tripleCallback_ = cb;
}

void AbstractInterface::clearUpdateCallback()
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    callback_ = nullptr;
}

void AbstractInterface::clearTripleUpdateCallback()
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    tripleCallback_ = nullptr;
}

void AbstractInterface::triggerCallback(
        callback_t::first_argument_type arg1,
        callback_t::second_argument_type arg2)
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    if (callback_) callback_(arg1, arg2);
}

void AbstractInterface::triggerTripleCallback(
        triple_callback_t::first_argument_type arg1,
        triple_callback_t::second_argument_type arg2)
{
    std::lock_guard<std::mutex> lg(callbackMutex_);
    if (tripleCallback_) tripleCallback_(arg1, arg2);
}

}}
