#ifndef SERVER_TEMPLATE_WS_WS_FRAME_BUILDER_H_
#define SERVER_TEMPLATE_WS_WS_FRAME_BUILDER_H_

#include "ws_message.h"
#include "ws_status.h"
#include <queue>

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketFrameBuilder
{
public:
    static int build(std::queue<WebsocketFrame *> &frameQueue, const WebsocketMessage &message, bool fragmentation, size_t mtu = UINT32_MAX, bool mask = false)
    {
        auto fragmentCount = fragmentation ? (size_t)std::ceil(message.getPayloadLength() / (double)mtu) : 1;
        if (fragmentCount > 1)
        {
            for (int i = 0; i < fragmentCount; i++)
            {
                frameQueue.push(buildFragment(message, i, mtu, mask));
            }
        }
        else
        {
            frameQueue.push(buildSingle(message, mask));
        }
        return fragmentCount;
    }

    static void buildCloseFrame(WebsocketFrame &frame, WebsocketStatus status = WebsocketStatus::CLOSURE, bool mask = false)
    {
        frame.init(WebsocketOpcode::CONNECTION_CLOSE);
        util::ByteConverter::toBytes((uint16_t)status, frame.payload);
        frame.payload.append(WebsocketStatusTool::getStatusString(status));
        if (mask)
        {
            frame.maskPayload();
        }
    }

    static void buildCloseFrame(WebsocketFrame &frame, const WebsocketFrame *closeFrame, bool mask = false)
    {
        frame.init(WebsocketOpcode::CONNECTION_CLOSE);
        frame.payload = closeFrame->payload;
        if (mask)
        {
            frame.maskPayload();
        }
    }

    static void buildPingFrame(WebsocketFrame &frame, const util::ByteArray &payload, bool mask = false)
    {
        frame.init(WebsocketOpcode::PING);
        frame.payload = payload;
        if (mask)
        {
            frame.maskPayload();
        }
    }

    static void buildPongFrame(WebsocketFrame &frame, const WebsocketFrame *ping, bool mask = false)
    {
        frame.init(WebsocketOpcode::PONG);
        frame.payload = ping->payload;
        if (mask)
        {
            frame.maskPayload();
        }
    }

private:
    static WebsocketFrame *buildSingle(const WebsocketMessage &message, bool mask = false)
    {
        auto frame = new WebsocketFrame();
        frame->init((WebsocketOpcode)message.type);
        frame->payload = message.payload;
        if (mask)
        {
            frame->maskPayload();
        }
        return frame;
    }

    static WebsocketFrame *buildFragment(const WebsocketMessage &message, size_t index, size_t mtu, bool mask = false)
    {
        auto frame = new WebsocketFrame();
        size_t offset = index * mtu;
        if (index == 0)
        {
            // first one
            frame->init((WebsocketOpcode)message.type, false);
            auto rsv = message.getRsv();
            frame->header.rsv1 = rsv[0];
            frame->header.rsv2 = rsv[1];
            frame->header.rsv3 = rsv[2];
        }
        else
        {
            frame->init(WebsocketOpcode::CONTINUATION_FRAME, false);
            if (offset + mtu >= message.getPayloadLength())
            {
                // last one
                frame->header.fin = true;
                mtu = message.getPayloadLength() - offset;
            }
        }
        frame->payload = util::ByteArray((char *)message.payload.data() + offset, mtu);
        if (mask)
        {
            frame->maskPayload();
        }
        return frame;
    }
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_FRAME_BUILDER_H_