#ifndef ENDPOINTS_ECHO_ENDPOINT_H_
#define ENDPOINTS_ECHO_ENDPOINT_H_

#include <server_template/ws/ws_endpoint.h>

using WebsocketSessionHandler = server_template::ws::WebsocketSessionHandler;
using WebsocketMessage = server_template::ws::WebsocketMessage;

class EchoEndpoint : public server_template::ws::WebsocketEndpoint
{
public:
    virtual void onConnectionOpen(WebsocketSessionHandler *handler) override
    {

    }

    virtual void onConnectionClose(WebsocketSessionHandler *handler) override
    {

    }

    virtual void onMessage(WebsocketSessionHandler *handler, WebsocketMessage *message) override
    {
        auto str = std::string((char*)message->payload.data(), message->payload.size());
        handler->sendText(str);
    }

    virtual void onHello(WebsocketSessionHandler *handler, WebsocketMessage *message)
    {

    }
};

#endif // !ENDPOINTS_ECHO_ENDPOINT_H_