#ifndef ECHO_PROTOCOL_H_
#define ECHO_PROTOCOL_H_

#include <server_template/tcp/tcp_server_template.h>
#include <string>
#include <iostream>
#include "echo_protocol_config.h"

class EchoProtocol : public server_template::tcp::TCPBasedProtocol, public EchoProtocolConfigurations
{
public:
    virtual void onTCPData(ssize_t nread, char *bytes) override
    {
        std::cout << echoPrefix << std::string(bytes, nread) << std::endl;
        connHandler->writeBytes(bytes, nread);
    }

    virtual void useConfig(server_template::base::ConfigurationBase *config) override
    {
        auto echoConfig = dynamic_cast<EchoProtocolConfig *>(config);
        echoConfig->configEcho(this);
    }

    virtual void useEchoPrefix(const char* prefix) override
    {
        this->echoPrefix = std::string(prefix);
    }

private:
    std::string echoPrefix;
};

#endif