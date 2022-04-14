#ifndef SERVER_TEMPLATE_WS_WEBSOCKET_H_
#define SERVER_TEMPLATE_WS_WEBSOCKET_H_

#include "ws_ns.h"
#include "../http/http_upgrade_protocol.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class Websocket : public http::HttpUpgradeProtocol
{
public:
    virtual void handleUpgrade(http::HttpRequest *req) override
    {
    }

    virtual void onTCPData(ssize_t nread, char *bytes) override
    {
    }

    virtual void onTCPConnectionClose() override
    {
    }

    virtual void useConfig(base::ConfigurationBase *config) override
    {
    }

private:
    void startHandshake(http::HttpRequest *req)
    {
        
    }
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WEBSOCKET_H_