#ifndef SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_
#define SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_

#include "tcp_ns.h"
#include "connection_handler_base.h"

SERVER_TEMPLATE_TCP_NAMESPACE_BEGIN

class TCPBasedProtocol
{
public:
    virtual void onTCPData(ConnectionHandlerBase* connHandler, ssize_t nread, char* bytes) {}

    virtual void onTCPConnectionOpen(ConnectionHandlerBase* connHandler) {}

    virtual void onTCPConnectionClose(ConnectionHandlerBase* connHandler) {}
};

SERVER_TEMPLATE_TCP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_