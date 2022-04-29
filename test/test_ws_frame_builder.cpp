#include <server_template/ws/ws_frame_builder.h>
#include <server_template/ws/ws_message_builder.h>

#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

using WebsocketFrame = server_template::ws::WebsocketFrame;
using WebsocketFrameBuilder = server_template::ws::WebsocketFrameBuilder;
using WebsocketMessageBuilder = server_template::ws::WebsocketMessageBuilder;
using WebsocketMessage = server_template::ws::WebsocketMessage;
using ByteArray = server_template::util::ByteArray;

void cleanQueue(std::queue<server_template::ws::WebsocketFrame *>& frameQueue)
{
    while (!frameQueue.empty())
    {
        delete frameQueue.front();
        frameQueue.pop();
    }
}

int main()
{
    ByteArray byteBuffer;

    // build ping
    WebsocketFrame pingFrame;
    WebsocketFrameBuilder::buildPingFrame(pingFrame, ByteArray("Hello"));
    pingFrame.toBytes(byteBuffer);
    const uint8_t pingFrameBytes[] = {0x89, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f};
    ASSERT(byteBuffer.equals(pingFrameBytes))

    // build pong
    WebsocketFrame pongFrame;
    WebsocketFrameBuilder::buildPongFrame(pongFrame, &pingFrame, false);
    pongFrame.toBytes(byteBuffer);
    const uint8_t pongFrameBytes[] = {0x8A, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f};
    ASSERT(byteBuffer.equals(pongFrameBytes))

    // build close
    WebsocketFrame closeFrame;
    WebsocketFrameBuilder::buildCloseFrame(closeFrame, WebsocketStatus::CLOSURE);
    closeFrame.toBytes(byteBuffer);
    const uint8_t closeFrameBytes[] = {0x88, 0x10, 0x03, 0xE8, 0x4E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x20, 0x43, 0x6C, 0x6F, 0x73, 0x75, 0x72, 0x65};
    ASSERT(byteBuffer.equals(closeFrameBytes))

    // build message
    std::vector<server_template::ws::WebsocketPMEInstance> extensions;
    WebsocketMessage message;
    WebsocketMessageBuilder::build(message, ByteArray("Hello"), WebsocketMessage::Type::TEXT, extensions);

    std::queue<server_template::ws::WebsocketFrame *> frameQueue;

    // build single frame
    WebsocketFrameBuilder::build(frameQueue, message, false);
    ASSERT(frameQueue.size() == 1)
    auto singleFrame = frameQueue.front();
    singleFrame->toBytes(byteBuffer);
    const uint8_t singleFrameBytes[] = {0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f};
    ASSERT(byteBuffer.equals(singleFrameBytes))

    // build fragments
    cleanQueue(frameQueue);
    WebsocketFrameBuilder::build(frameQueue, message, true, 3);
    ASSERT(frameQueue.size() == 2)
    auto frag1 = frameQueue.front();
    frag1->toBytes(byteBuffer);
    const uint8_t frag1Bytes[] = {0x01, 0x03, 0x48, 0x65, 0x6c};
    ASSERT(byteBuffer.equals(frag1Bytes))
    auto frag2 = frameQueue.back();
    frag2->toBytes(byteBuffer);
    const uint8_t frag2Bytes[] = {0x80, 0x02, 0x6c, 0x6f};
    ASSERT(byteBuffer.equals(frag2Bytes))

    cleanQueue(frameQueue);
}