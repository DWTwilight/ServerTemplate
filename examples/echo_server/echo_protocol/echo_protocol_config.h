#ifndef ECHO_PROTOCOL_CONFIG_H_
#define ECHO_PROTOCOL_CONFIG_H_

#include "echo_protocol.h"

class EchoProtocol;
class EchoProtocolConfig
{
public:
    virtual void configEcho(EchoProtocol *protocol) = 0;
};

#endif