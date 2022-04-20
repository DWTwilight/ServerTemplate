#ifndef SERVER_TEMPLATE_WS_WS_FRAME_H_
#define SERVER_TEMPLATE_WS_WS_FRAME_H_

#include "ws_ns.h"
#include "../util/byte_converter.h"
#include "../util/uuid.h"

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

    void init(WebsocketOpcode opcode, bool fin = true)
    {
        header.fin = fin;
        header.opcode = opcode;
        header.rsv1 = false;
        header.rsv2 = false;
        header.rsv3 = false;
        header.mask = false;
        maskKey = util::ByteArray(4);
        payload = util::ByteArray();
    }

    size_t getPayloadLength() const
    {
        return payload.empty() ? payload.capacity() : payload.size();
    }

    bool isControlFrame() const
    {
        return header.opcode == WebsocketOpcode::CONNECTION_CLOSE || header.opcode == WebsocketOpcode::PING || header.opcode == WebsocketOpcode::PONG;
    }

    void maskPayload()
    {
        this->header.mask = true;
        this->maskKey = util::ByteArray(util::UUID::generate(4));
        // mask payload
        for (int i = 0; i < this->payload.size(); i++)
        {
            this->payload[i] ^= this->maskKey[i % 4];
        }
    }

    /**
     * @brief convert this frame to byte array; the frame should be extension-encoded already(if extensions are enabled) or masked
     *
     * @param bytes output buffer
     */
    void toBytes(util::ByteArray &bytes)
    {
        // cal payload length
        auto length = this->getPayloadLength();
        util::ByteArray buffer;
        if (length <= 125)
        {
            this->header.payloadLength = length;
        }
        else if (length <= UINT16_MAX)
        {
            this->header.payloadLength = 126;
            auto len16 = (uint16_t)length;
            util::ByteConverter::toBytes(len16, buffer);
        }
        else
        {
            this->header.payloadLength = 127;
            auto len64 = (uint64_t)length;
            util::ByteConverter::toBytes(len64, buffer);
        }
        bytes = util::ByteArray(2 + buffer.size() + maskKey.size() + payload.size());
        bytes.append((char *)&this->header, 2);
        bytes.append(buffer);
        if (this->header.mask)
        {
            bytes.append(maskKey);
        }
        bytes.append(payload);
    }
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_FRAME_H_