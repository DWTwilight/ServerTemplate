#ifndef SERVER_TEMPLATE_HTTP_HTTP_H_
#define SERVER_TEMPLATE_HTTP_HTTP_H_

#include "http_request_parser.h"
#include "http_config.h"
#include "../tcp/tcp_based_protocol.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

/**
 * @brief This is not a complete implementation of http protocol;
 * only http-upgrade protocols are supported;
 * for other requests, it just returns 400 Bad Request
 */
class Http : public tcp::TCPBasedProtocol, public HttpConfigurations
{
public:
    virtual ~Http()
    {
        if (this->upgradeFactory != NULL)
        {
            delete this->upgradeFactory;
        }
    }

    virtual void onTCPData(ssize_t nread, char *bytes) override 
    {

    }

    virtual void onTCPConnectionClose() override 
    {

    }

    virtual void useConfig(base::ConfigurationBase *config) override
    {
        auto httpConfig = dynamic_cast<HttpConfig*>(config);
        httpConfig->configHttp(this);
    }

    virtual void setMaxContentLength(uint64_t value) override
    {
        this->maxContentLength = value;
    }

    virtual void setUpgradeFactoryBuilder(const util::Builder<HttpUpgradeFactory>& factoryBuilder) override
    {
        this->upgradeFactory = factoryBuilder.build();
    }

private:
    uint64_t maxContentLength;
    HttpUpgradeFactory* upgradeFactory = NULL;
    HttpUpgradeProtocol* upgrade = NULL;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_H_