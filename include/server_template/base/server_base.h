#ifndef SERVER_TEMPLATE_BASE_SERVER_BASE_H_
#define SERVER_TEMPLATE_BASE_SERVER_BASE_H_

#include "config_base.h"
#include <uv.h>

SERVER_TEMPLATE_BASE_NAMESPACE_BEGIN

class ServerBase : public ServerConfigurations
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

    virtual void run(uv_loop_t* loop) = 0;

protected:
    ConfigurationBase *config = NULL;
};

SERVER_TEMPLATE_BASE_NAMESPACE_END

#endif // !SERVER_TEMPLATE_BASE_SERVER_BASE_H_