#ifndef SERVER_TEMPLATE_BASE_SERVER_BASE_H_
#define SERVER_TEMPLATE_BASE_SERVER_BASE_H_

#include "config_base.h"

SERVER_TEMPLATE_BASE_NAMESPACE_BEGIN

class IServerBase
{
public:
    virtual void run() = 0;

    virtual void config(ConfigurationBase& config) = 0;
};

SERVER_TEMPLATE_BASE_NAMESPACE_END


#endif // !SERVER_TEMPLATE_BASE_SERVER_BASE_H_