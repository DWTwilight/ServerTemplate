#ifndef SERVER_TEMPLATE_BASE_CONFIG_BASE_H_
#define SERVER_TEMPLATE_BASE_CONFIG_BASE_H_

#include "base_ns.h"
#include "../util/ip_address.h"

SERVER_TEMPLATE_BASE_NAMESPACE_BEGIN

class ServerConfigurations
{
public:
    virtual void useIpAddress(util::IpAddress& ipAddress) = 0;
};

class ConfigurationBase
{
public:
    virtual void configServer(ServerConfigurations* server) = 0;
};

SERVER_TEMPLATE_BASE_NAMESPACE_END

#endif // !SERVER_TEMPLATE_BASE_CONFIG_BASE_H_