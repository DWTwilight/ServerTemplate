#ifndef SERVER_TEMPLATE_WS_WS_FRAME_H_
#define SERVER_TEMPLATE_WS_WS_FRAME_H_

#include "ws_ns.h"
#include "../util/byte_array.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

enum class WebsocketOpcode : uint8_t
{
    CONTINUATION_FRAME = 0,
    TEXT_FRAME = 1,
    BINARY_FRAME = 2,
    CONNECTION_CLOSE = 8,
    PING = 9,
    PONG = 10,
};

struct WebsocketFrameHeader
{
    WebsocketOpcode opcode : 4;
    bool rsv3 : 1;
    bool rsv2 : 1;
    bool rsv1 : 1;
    bool fin : 1;

    uint8_t payloadLength : 7;
    bool mask : 1;
};

class WebsocketFrame
{
public:
    WebsocketFrameHeader header;
    util::ByteArray maskKey = util::ByteArray(4);
    util::ByteArray payload;

    size_t getPayloadLength() const
    {
        return payload.empty() ? payload.capacity() : payload.size();
    }

    bool isControlFrame() const
    {
        return header.opcode == WebsocketOpcode::CONNECTION_CLOSE || header.opcode == WebsocketOpcode::PING || header.opcode == WebsocketOpcode::PONG;
    }
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_FRAME_H_