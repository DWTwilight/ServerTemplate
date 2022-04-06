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
        std::cout << this->connHandler->getClientIpAddress()->getSocketAddress() << echoPrefix << std::string(bytes, nread) << std::endl;
        connHandler->writeBytes(bytes, nread);
    }

    virtual void onTCPConnectionOpen() override 
    {
        std::cout << this->connHandler->getClientIpAddress()->getSocketAddress() << " has connected" << std::endl;
    }

    virtual void onTCPConnectionClose() override 
    {
        std::cout << this->connHandler->getClientIpAddress()->getSocketAddress() << " has disconnected" << std::endl;
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