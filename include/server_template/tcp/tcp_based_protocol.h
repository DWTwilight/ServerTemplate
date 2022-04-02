#ifndef SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_
#define SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_

#include "tcp_ns.h"

SERVER_TEMPLATE_TCP_NAMESPACE_BEGIN

class TCPBasedProtocol
{
public:
    virtual void onTCPData() {}

    virtual void onTCPConnectionOpen() {}

    virtual void onTCPConnectionClose() {}
};

SERVER_TEMPLATE_TCP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_TCP_TCP_BASED_PROTOCOL_H_