#ifndef SERVER_TEMPLATE_WS_WS_STATUS_H_
#define SERVER_TEMPLATE_WS_WS_STATUS_H_

#include "ws_ns.h"
#include <string>
#include <stdint.h>

#define WS_STATUS_MAP(XX)                                     \
    XX(1000, CLOSURE, Normal Closure)                         \
    XX(1001, GOING_AWAY, Endpoint is Going Away)              \
    XX(1002, PROTOCOL_ERROR, Protocol Error)                  \
    XX(1003, UNACCEPTABLE_TYPE, Data Type is not Acceptable)  \
    XX(1007, INCONSISTENT, Message is not of Consistent Type) \
    XX(1008, POLICY_VIOLATION, Message Violating Policy)      \
    XX(1009, MESSAGE_TOO_BIG, Message is too Big)             \
    XX(1010, EXTENSION_ERROR, Extension Error)                \
    XX(0, UNKNOWN, Unknown)

enum class WebsocketStatus : uint16_t
{
#define XX(num, name, string) name = num,
    WS_STATUS_MAP(XX)
#undef XX
};

class WebsocketStatusTool
{
public:
    static const char *getStatusString(const WebsocketStatus &status)
    {
        switch (status)
        {
#define XX(num, name, str)      \
    case WebsocketStatus::name: \
        return #str;
            WS_STATUS_MAP(XX)
#undef XX
        default:
            return "";
        }
    }
};

#endif // !SERVER_TEMPLATE_WS_WS_STATUS_H_