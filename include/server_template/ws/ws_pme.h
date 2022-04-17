#ifndef SERVER_TEMPLATE_WS_WS_PME_H_
#define SERVER_TEMPLATE_WS_WS_PME_H_

#include "ws_extension.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketPerMessageExtension : public WebsocketExtension
{
public:
    WebsocketPerMessageExtension(int rsvIndex, const std::string &name, const std::string &paramName)
        : WebsocketExtension(rsvIndex, name, paramName, WebsocketExtension::Type::PER_MESSAGE) {}

    /**
     * @brief decode the message using this extension's algorithm
     *
     * @param message
     * @return true : success
     * @return false : failed, the connection will close afterwards
     */
    virtual bool decodeMessage(WebsocketMessage &message, const std::string &param) = 0;

    /**
     * @brief encode the message to send
     *
     * @param message the message to send
     */
    virtual void encodeMessage(WebsocketMessage &message, const std::string &param) = 0;
};

class WebsocketPMEInstance
{
public:
    WebsocketPMEInstance(WebsocketPerMessageExtension *extension, const std::string &param = std::string())
    {
        this->extension = extension;
        this->param = param;
    }

    void setParam(const std::string &param)
    {
        this->param = param;
    }

    bool decodeMessage(WebsocketMessage &message)
    {
        return extension->decodeMessage(message, param);
    }

    void encodeMessage(WebsocketMessage &message)
    {
        extension->encodeMessage(message, param);
    }

    int getRsvIndex() const
    {
        return extension->getRsvIndex();
    }

private:
    WebsocketPerMessageExtension *extension;
    std::string param;
};

class PerMessageCompressionExtension : public WebsocketPerMessageExtension
{
public:
    PerMessageCompressionExtension(const std::string &name, const std::string &paramName)
        : WebsocketPerMessageExtension(0, name, paramName) {}
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_PME_H_