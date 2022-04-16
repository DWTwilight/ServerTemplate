#ifndef SERVER_TEMPLATE_HTTP_HTTP_CONFIG_H_
#define SERVER_TEMPLATE_HTTP_HTTP_CONFIG_H_

#include "http_ns.h"
#include "http_upgrade_factory.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

class HttpConfigurations
{
public:
    virtual void setMaxContentLength(size_t value) = 0;
};

class HttpConfig
{
public:
    virtual void configHttp(HttpConfigurations *protocol) = 0;

    virtual void configUpgradeFactoryBuilder(HttpUpgradeFactoryBuilder& builder) = 0;

    const util::Builder<HttpUpgradeFactory>& getHttpUpgradeFactoryBuilder() const
    {
        return this->upgradeFactoryBuilder;
    }

protected:
    HttpUpgradeFactoryBuilder upgradeFactoryBuilder;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif