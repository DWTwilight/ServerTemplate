#ifndef SERVER_TEMPLATE_WS_WEBSOCKET_H_
#define SERVER_TEMPLATE_WS_WEBSOCKET_H_

#include "ws_config.h"
#include "ws_message_parser.h"
#include "../http/http_upgrade_protocol.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class Websocket : public http::HttpUpgradeProtocol, public WebsocketConfigurations, public WebsocketSessionHandler
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
        auto wsConfig = dynamic_cast<WebsocketConfig *>(config);
        wsConfig->initialize();
        wsConfig->configWebsocket(this);
        this->endpointManager = wsConfig->getEndpointManager();
        this->pmeManager = wsConfig->getPMEManager();
    }

    virtual void setMaxPayloadLength(size_t value) override
    {
    }

    virtual void closeConnection() override
    {

    }

    virtual void sendMessage(util::ByteArray& bytes, WebsocketMessage::Type type, bool fragmentation = false, size_t mtu = UINT16_MAX) override
    {

    }

    virtual const WebsocketSessionInfo* getSessionInfo() const override
    {
        return &this->sessionInfo;
    }

    void onControlFrame(WebsocketFrame *)
    {
    }

    void onMessage(WebsocketMessage *)
    {
    }

    const std::vector<WebsocketPerMessageExtension *>* getPMEExtensions() const
    {
        return &this->sessionInfo.pmeExtensions;
    }

    const WebsocketMessageParser::Config* getParserConfig() const
    {
        return &this->config;
    }

private:
    void startHandshake(http::HttpRequest *req)
    {
    }

    // global config
    const WebsocketEndpointManager *endpointManager;
    const WebsocketPMEManager *pmeManager;

    // session info
    WebsocketSessionInfo sessionInfo;

    // endpoint
    WebsocketEndpoint* endpoint;

    // parser
    WebsocketMessageParser::Config config{this,
                                          [](void *arg, WebsocketFrame *frame)
                                          {
                                              ((Websocket *)arg)->onControlFrame(frame);
                                          },
                                          [](void *arg, WebsocketMessage *msg)
                                          {
                                              ((Websocket *)arg)->onMessage(msg);
                                          }};
    WebsocketMessageParser parser = WebsocketMessageParser(this->getPMEExtensions(), this->getParserConfig());
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WEBSOCKET_H_