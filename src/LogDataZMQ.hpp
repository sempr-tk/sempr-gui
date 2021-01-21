#ifndef SEMPR_GUI_LOGDATAZMQ_HPP_
#define SEMPR_GUI_LOGDATAZMQ_HPP_

#include "AbstractInterface.hpp"
#include <zmqpp/zmqpp.hpp>

namespace sempr { namespace gui {

    // pushing LogData into a message
    inline zmqpp::message& operator << (zmqpp::message& msg, const LogData& data)
    {
        msg
            << static_cast<size_t>(data.level)
            << data.name
            << data.message
            << data.timestamp.time_since_epoch().count();
        return msg;
    }

    // getting LogData from a message
    inline zmqpp::message& operator >> (zmqpp::message& msg, LogData& data)
    {
        size_t level;
        msg
            >> level
            >> data.name
            >> data.message;

        data.level = static_cast<LogData::Level>(level);

        LogData::sys_time::duration::rep count;

        msg >> count;

        data.timestamp = LogData::sys_time(LogData::sys_time::duration(count));
        return msg;
    }

}}

#endif /* include guard: SEMPR_GUI_LOGDATAZMQ_HPP_ */
