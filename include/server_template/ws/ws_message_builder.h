#ifndef SERVER_TEMPLATE_WS_WS_MESSAGE_BUILDER_H_
#define SERVER_TEMPLATE_WS_WS_MESSAGE_BUILDER_H_

#include "ws_message.h"
#include "ws_pme.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketMessageBuilder
{
public:
    static void build(WebsocketMessage& message, const util::ByteArray &bytes, WebsocketMessage::Type type, std::vector<WebsocketPMEInstance>& extensions)
    {
        message.payload = bytes;
        message.type = type;
        message.complete = true;
        for (auto& extension : extensions)
        {
            extension.encodeMessage(message);
            message.rsv[extension.getRsvIndex()] = true;
        }
    }
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_MESSAGE_BUILDER_H_