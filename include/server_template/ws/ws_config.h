#ifndef SERVER_TEMPLATE_WS_WS_CONFIG_H_
#define SERVER_TEMPLATE_WS_WS_CONFIG_H_

#include "ws_endpoint_manager.h"
#include "ws_pme_manager.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketConfigurations
{
public:
    virtual void setMaxPayloadLength(size_t value) = 0;
};

class WebsocketConfig
{
public:
    /**
     * @brief config basic websocket properties
     *
     * @param protocol properties to config
     */
    virtual void configWebsocket(WebsocketConfigurations *protocol) = 0;

    /**
     * @brief config websocket endpoints
     *
     * @param endpointManager websocket endpoints manager
     */
    virtual void configEndpoint(WebsocketEndpointManager &endpointManager) = 0;

    /**
     * @brief config websocket per-message extensions
     *
     * @param pmeManager websocket per-message extensions manager
     */
    virtual void configPME(WebsocketPMEManager &pmeManager) = 0;

    void initialize()
    {
        if (init)
        {
            configEndpoint(this->endpointManager);
            configPME(this->pmeManager);
            init = false;
        }
    }

    WebsocketEndpointManager *getEndpointManager()
    {
        return &this->endpointManager;
    }

    WebsocketPMEManager *getPMEManager()
    {
        return &this->pmeManager;
    }

private:
    bool init = true;
    WebsocketEndpointManager endpointManager;
    WebsocketPMEManager pmeManager;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_CONFIG_H_