#ifndef SERVER_TEMPLATE_BASE_CONFIG_BASE_H_
#define SERVER_TEMPLATE_BASE_CONFIG_BASE_H_

#include "base_ns.h"

SERVER_TEMPLATE_BASE_NAMESPACE_BEGIN

class ServerBase;
class ConfigurationBase
{
public:
    virtual void configServer(ServerBase* server) = 0;
};

SERVER_TEMPLATE_BASE_NAMESPACE_END

#endif // !SERVER_TEMPLATE_BASE_CONFIG_BASE_H_