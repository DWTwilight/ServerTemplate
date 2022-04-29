#ifndef SERVER_TEMPLATE_HTTP_HTTP_UPGRADE_FACTORY_H_
#define SERVER_TEMPLATE_HTTP_HTTP_UPGRADE_FACTORY_H_

#include "http_upgrade_protocol.h"
#include "../util/builder.h"
#include <map>
#include <vector>

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

class HttpUpgradeFactory
{
public:
    HttpUpgradeFactory(const std::map<std::string, HttpUpgradeProtocol::UpgradeProtocolFactory> *factoryMap)
    {
        this->factoryMap = factoryMap;
    }

    HttpUpgradeProtocol *getProtocol(const std::string &name, base::ConfigurationBase *config, tcp::ConnectionHandlerBase *connHandler)
    {
        if (this->factoryMap->find(name) == this->factoryMap->end())
        {
            return NULL;
        }
        auto instance = this->factoryMap->at(name)(config, connHandler);
        return instance;
    }

private:
    const std::map<std::string, HttpUpgradeProtocol::UpgradeProtocolFactory> *factoryMap;
};

class HttpUpgradeFactoryBuilder : public util::Builder<HttpUpgradeFactory>
{
public:
    virtual HttpUpgradeFactory *build() const override
    {
        return new HttpUpgradeFactory(&factoryMap);
    }

    template <typename Upgrade>
    void addUpgrade(const std::string &name)
    {
        factoryMap[name] = [](base::ConfigurationBase *config, tcp::ConnectionHandlerBase *connHandler)
        {
            auto protocol = dynamic_cast<HttpUpgradeProtocol *>(new Upgrade());
            protocol->setConnectionHandler(connHandler);
            protocol->useConfig(config);
            return protocol;
        };
    }

private:
    std::map<std::string, HttpUpgradeProtocol::UpgradeProtocolFactory> factoryMap;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_UPGRADE_FACTORY_H_