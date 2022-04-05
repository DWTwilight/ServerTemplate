#ifndef SERVER_TEMPLATE_BASE_SERVER_BASE_H_
#define SERVER_TEMPLATE_BASE_SERVER_BASE_H_

#include "config_base.h"
#include <uv.h>
#include "../util/ip_address.h"

SERVER_TEMPLATE_BASE_NAMESPACE_BEGIN

class ServerBase
{
public:
    virtual ~ServerBase()
    {
        if (this->config != NULL)
        {
            delete this->config;
        }
    }

    template <typename ConfigClass>
    void useConfigClass()
    {
        this->config = dynamic_cast<ConfigurationBase *>(new ConfigClass());
    }

    virtual void run() = 0;
    virtual void useLoop(uv_loop_t* loop) = 0;
    virtual void useIpAddress(util::IpAddress& ipAddress) = 0;

protected:
    ConfigurationBase *config = NULL;
};

SERVER_TEMPLATE_BASE_NAMESPACE_END

#endif // !SERVER_TEMPLATE_BASE_SERVER_BASE_H_