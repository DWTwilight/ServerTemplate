#ifndef SERVER_TEMPLATE_WS_WS_ENDPOINT_H_
#define SERVER_TEMPLATE_WS_WS_ENDPOINT_H_

#include "ws_session_handler.h"
#include <map>
#include <unordered_map>
#include <functional>

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

using SubprotocolHandler = std::function<void(WebsocketSessionHandler *handler, WebsocketMessage *message)>;

class WebsocketEndpoint
{
public:
    virtual bool preAuthorize(http::HttpRequest* req, std::map<std::string, std::string> &securityAttributes)
    {
        return true;
    }

    virtual void onConnectionOpen(WebsocketSessionHandler *handler) = 0;

    virtual void onConnectionClose(WebsocketSessionHandler *handler) = 0;

    virtual void onMessage(WebsocketSessionHandler *handler, WebsocketMessage *message) = 0;

    WebsocketEndpoint *addSubprotocolHandler(const std::string &name, SubprotocolHandler handler)
    {
        this->subprotocolHandlerMap[name] = handler;
        return this;
    }

protected:
    std::unordered_map<std::string, SubprotocolHandler> subprotocolHandlerMap;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_ENDPOINT_H_