#ifndef SERVER_TEMPLATE_WS_WS_CONFIG_H_
#define SERVER_TEMPLATE_WS_WS_CONFIG_H_

#include "ws_endpoint_manager.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketConfigurations
{
public:
    virtual void setMaxPayloadLength(size_t value) = 0;
};

class WebsocketConfig
{
public:
    virtual void configWebsocket(WebsocketConfigurations *protocol) = 0;

    virtual void configEndpoint(WebsocketEndpointManager &endpointManager) = 0;

    void initialize()
    {
        if (init)
        {
            configEndpoint(this->endpointManager);
            init = false;
        }
    }

private:
    bool init = true;
    WebsocketEndpointManager endpointManager;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_CONFIG_H_