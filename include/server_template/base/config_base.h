#ifndef SERVER_TEMPLATE_BASE_CONFIG_BASE_H_
#define SERVER_TEMPLATE_BASE_CONFIG_BASE_H_

#include "base_ns.h"
#include "../util/ip_address.h"

SERVER_TEMPLATE_BASE_NAMESPACE_BEGIN

class ServerConfigurations
{
public:
    virtual void useIpAddress(const util::IpAddress& ipAddress) = 0;
    virtual void setMaxConnectionCount(uint32_t value) = 0;
};

class ConfigurationBase
{
public:
    virtual ~ConfigurationBase() {}

    virtual void configServer(ServerConfigurations* server) = 0;
};

SERVER_TEMPLATE_BASE_NAMESPACE_END

#endif // !SERVER_TEMPLATE_BASE_CONFIG_BASE_H_