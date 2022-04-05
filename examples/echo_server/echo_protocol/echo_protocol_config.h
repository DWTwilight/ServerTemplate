#ifndef ECHO_PROTOCOL_CONFIG_H_
#define ECHO_PROTOCOL_CONFIG_H_

#include "echo_protocol.h"

class EchoProtocolConfigurations
{
public:
    virtual void useEchoPrefix(const char* prefix) = 0;
};

class EchoProtocolConfig
{
public:
    virtual void configEcho(EchoProtocolConfigurations *protocol) = 0;
};

#endif