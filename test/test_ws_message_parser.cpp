#include <server_template/ws/ws_message_parser.h>

#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

using WebsocketPMEInstance = server_template::ws::WebsocketPMEInstance;
using WebsocketMessageParser = server_template::ws::WebsocketMessageParser;
using WebsocketFrame = server_template::ws::WebsocketFrame;
using WebsocketMessage = server_template::ws::WebsocketMessage;

const uint8_t bytes1[] = {0x01, 0x03, 0x48, 0x65, 0x6c};             // frag0
const uint8_t bytes2[] = {0x89, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f}; // ping
const uint8_t bytes3[] = {0x80, 0x02, 0x6c, 0x6f}; // frag1

int controlFrameCount = 0;
int messageCount = 0;
WebsocketMessage message;
WebsocketFrame frame;

int main()
{
    WebsocketMessageParser::Config config{
        NULL,
        [](void *, WebsocketFrame *frm)
        {
            controlFrameCount++;
            frame = *frm;
        },
        [](void *, WebsocketMessage *msg)
        {
            messageCount++;
            message = *msg;
        }};
    std::vector<WebsocketPMEInstance> extensions;
    auto parser = WebsocketMessageParser(&extensions, &config, UINT32_MAX);
    size_t nparsed;
    auto res = parser.parse((char*)bytes1, (char*)bytes1 + 5, nparsed);
    ASSERT(res == server_template::base::ParseResult::INCOMPLETE)
    ASSERT(nparsed == 5)

    res = parser.parse((char*)bytes2, (char*)bytes2 + 7, nparsed);
    ASSERT(res == server_template::base::ParseResult::INCOMPLETE)
    ASSERT(nparsed == 7)
    ASSERT(controlFrameCount == 1)
    ASSERT(frame.header.opcode == server_template::ws::WebsocketOpcode::PING)

    res = parser.parse((char*)bytes3, (char*)bytes3 + 4, nparsed);
    ASSERT(res == server_template::base::ParseResult::INCOMPLETE)
    ASSERT(nparsed == 4)
    ASSERT(messageCount == 1)
    ASSERT(message.complete)
    ASSERT(message.frameCount == 2)
    ASSERT(message.type == WebsocketMessage::Type::TEXT)
    ASSERT(message.payload.equals("Hello"))
}