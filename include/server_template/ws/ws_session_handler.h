#ifndef SERVER_TEMPLATE_WS_WS_SESSION_HANDLER_H_
#define SERVER_TEMPLATE_WS_WS_SESSION_HANDLER_H_

#include "ws_pme.h"
#include "ws_message.h"
#include "../util/byte_array.h"
#include "../util/ip_address.h"
#include "../http/http_request.h"
#include <map>

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN
class WebsocketSessionInfo
{
public:
    std::string sessionKey;
    std::vector<WebsocketPMEInstance> pmeExtensions;
    std::string subprotocol;
    util::IpAddress* clientIpAddress;
    std::map<std::string, std::string> securityAttributes;
    http::HttpRequest handshakeRequest;
};

class WebsocketSessionHandler
{
public:
    /**
     * @brief close websocket connection with the client
     * 
     */
    virtual void closeConnection() = 0;

    /**
     * @brief send message to client
     * 
     * @param bytes bytes to send
     * @param type text | binary
     * @param fragmentation enable fragmentation? default false
     * @param mtu max transport uint (frame payload length, in bytes) default 0xffffui16
     */
    virtual void sendMessage(util::ByteArray& bytes, WebsocketMessage::Type type, bool fragmentation = false, size_t mtu = UINT16_MAX) = 0;

    /**
     * @brief send utf-8 encoded text to client
     * 
     * @param bytes bytes to send
     * @param fragmentation enable fragmentation? default false
     * @param mtu max transport uint (frame payload length, in bytes) default 0xffffui16
     */
    void sendText(const std::string& text, bool fragmentation = false, size_t mtu = UINT16_MAX)
    {
        auto byteArray = util::ByteArray(text);
        sendMessage(byteArray, WebsocketMessage::Type::TEXT, fragmentation, mtu);
    }

    /**
     * @brief send binary bytes to client
     * 
     * @param bytes bytes pointer
     * @param length number of bytes to send
     * @param fragmentation enable fragmentation? default false
     * @param mtu max transport uint (frame payload length, in bytes) default 0xffffui16
     */
    void sendBinary(const char* bytes, size_t length, bool fragmentation = false, size_t mtu = UINT16_MAX)
    {
        auto byteArray = util::ByteArray(bytes, length);
        sendMessage(byteArray, WebsocketMessage::Type::BINARY, fragmentation, mtu);
    }

    /**
     * @brief send binary bytes to client
     * 
     * @param bytes bytes to send
     * @param fragmentation enable fragmentation? default false
     * @param mtu max transport uint (frame payload length, in bytes) default 0xffffui16
     */
    void sendBinary(const util::ByteArray& bytes, bool fragmentation = false, size_t mtu = UINT16_MAX)
    {
        auto byteArray = bytes;
        sendMessage(byteArray, WebsocketMessage::Type::BINARY, fragmentation, mtu);
    }

    /**
     * @brief Get the Session Info
     * 
     * @return const WebsocketSessionInfo* 
     */
    virtual const WebsocketSessionInfo* getSessionInfo() const = 0;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_SESSION_HANDLER_H_