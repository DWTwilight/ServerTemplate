#ifndef SERVER_TEMPLATE_WS_PMCE_H_
#define SERVER_TEMPLATE_WS_PMCE_H_

#include "ws_extension.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class PerMessageCompressionExtension : public WebsocketExtension
{
public:
    PerMessageCompressionExtension(const std::string &name, const std::string &paramName)
        : WebsocketExtension(0, name, paramName) {}

    /**
     * @brief decode the message using this extension's algorithm
     *
     * @param message
     * @return true : success
     * @return false : failed, the connection will close afterwards
     */
    virtual bool decodeMessage(WebsocketMessage &message) = 0;

    /**
     * @brief encode the message to send
     *
     * @param message the message to send
     */
    virtual void encodeMessage(WebsocketMessage &message) = 0;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_PMCE_H_