#ifndef SERVER_TEMPLATE_WS_WS_PME_H_
#define SERVER_TEMPLATE_WS_WS_PME_H_

#include "ws_extension.h"
#include <map>

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketPerMessageExtension : public WebsocketExtension
{
public:
    virtual ~WebsocketPerMessageExtension() {}

    WebsocketPerMessageExtension(int rsvIndex, const std::string &name)
        : WebsocketExtension(rsvIndex, name, WebsocketExtension::Type::PER_MESSAGE) {}

    /**
     * @brief decode the message using this extension's algorithm
     *
     * @param message
     * @return true : success
     * @return false : failed, the connection will close afterwards
     */
    virtual bool decodeMessage(WebsocketMessage &message, const std::map<std::string, std::string> &param) = 0;

    /**
     * @brief encode the message to send
     *
     * @param message the message to send
     */
    virtual void encodeMessage(WebsocketMessage &message, const std::map<std::string, std::string> &param) = 0;
};

class WebsocketPMEInstance
{
public:
    WebsocketPMEInstance(WebsocketPerMessageExtension *extension, const std::string& originalToken)
    {
        this->extension = extension;
        this->originalToken = originalToken;
    }

    void setParam(const std::map<std::string, std::string> &param)
    {
        this->param = param;
    }

    void addParam(const std::string& key, const std::string& value)
    {
        this->param[key] = value;
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

    const std::string& getOriginalToken() const
    {
        return this->originalToken;
    }

private:
    WebsocketPerMessageExtension *extension;
    std::string originalToken;
    std::map<std::string, std::string> param;
};

class PerMessageCompressionExtension : public WebsocketPerMessageExtension
{
public:
    virtual ~PerMessageCompressionExtension() {}

    PerMessageCompressionExtension(const std::string &name)
        : WebsocketPerMessageExtension(0, name) {}
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_PME_H_