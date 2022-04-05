#ifndef SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_
#define SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_

#include "tcp_ns.h"
#include "connection_handler_base.h"
#include "../base/config_base.h"
#include <functional>
#include <uv.h>

SERVER_TEMPLATE_TCP_NAMESPACE_BEGIN

class TCPBasedProtocol
{
public:
    using ProtocolFactory = std::function<TCPBasedProtocol *(base::ConfigurationBase*)>;

    virtual void onTCPData(ConnectionHandlerBase* connHandler, ssize_t nread, char* bytes) {}

    virtual void onTCPConnectionOpen(ConnectionHandlerBase* connHandler) {}

    virtual void onTCPConnectionClose(ConnectionHandlerBase* connHandler) {}

    virtual void useConfig(base::ConfigurationBase* config) = 0;
};

SERVER_TEMPLATE_TCP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_