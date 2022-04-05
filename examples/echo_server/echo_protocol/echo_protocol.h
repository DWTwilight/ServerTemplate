#ifndef ECHO_PROTOCOL_H_
#define ECHO_PROTOCOL_H_

#include <server_template/tcp/tcp_server_template.h>
#include <string>
#include <iostream>
#include "echo_protocol_config.h"

class EchoProtocol : public server_template::tcp::TCPBasedProtocol
{
public:
    virtual void onTCPData(server_template::tcp::ConnectionHandlerBase *connHandler, ssize_t nread, char *bytes) override
    {
        std::cout << echoPrefix.append(bytes, nread) << std::endl;
    }

    virtual void useConfig(server_template::base::ConfigurationBase *config) override
    {
        auto echoConfig = dynamic_cast<EchoProtocolConfig *>(config);
        echoConfig->configEcho(this);
    }

    void useEchoPrefix(const char* prefix)
    {
        this->echoPrefix = std::string(prefix);
    }

private:
    std::string echoPrefix;
};

#endif