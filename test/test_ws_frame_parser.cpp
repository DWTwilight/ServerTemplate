#include <server_template/ws/ws_frame_parser.h>

#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

using WebsocketFrameParser = server_template::ws::WebsocketFrameParser;
using WebsocketOpcode = server_template::ws::WebsocketOpcode;

const uint8_t bytes1[] = {0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f};
const uint8_t bytes2[] = {0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58};
const uint8_t bytes3[] = {0x82, 0x7E, 0x01, 0x00};
const uint8_t bytes4[] = {0x82, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
const uint8_t bytes5[] = {0xC1, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f};

int main()
{
    WebsocketFrameParser parser;
    auto frame = parser.getFrame();
    size_t nparsed;

    // bytes1, "Hello", no mask
    auto res = parser.parse((char *)bytes1, (char *)bytes1 + 7, nparsed);
    ASSERT(res == server_template::base::ParseResult::COMPLETE)
    ASSERT(frame->header.fin)
    ASSERT(!frame->header.mask)
    ASSERT(frame->header.opcode == WebsocketOpcode::TEXT_FRAME)
    ASSERT(frame->getPayloadLength() == 5)
    ASSERT(frame->payload.equals("Hello"))

    // bytes2, "Hello", with mask
    parser = WebsocketFrameParser();
    frame = parser.getFrame();
    res = parser.parse((char *)bytes2, (char *)bytes2 + 11, nparsed);
    ASSERT(res == server_template::base::ParseResult::COMPLETE)
    ASSERT(frame->header.fin)
    ASSERT(frame->header.mask)
    ASSERT(frame->header.opcode == WebsocketOpcode::TEXT_FRAME)
    ASSERT(frame->getPayloadLength() == 5)
    ASSERT(frame->payload.equals("Hello"))

    // bytes3, payload length = 256B, no mask
    parser = WebsocketFrameParser();
    frame = parser.getFrame();
    res = parser.parse((char *)bytes3, (char *)bytes3 + 4, nparsed);
    ASSERT(res == server_template::base::ParseResult::INCOMPLETE)
    ASSERT(frame->header.fin)
    ASSERT(!frame->header.mask)
    ASSERT(frame->header.opcode == WebsocketOpcode::BINARY_FRAME)
    ASSERT(frame->getPayloadLength() == 256)

    // bytes4, payload length = 64KB, no mask
    parser = WebsocketFrameParser();
    frame = parser.getFrame();
    res = parser.parse((char *)bytes4, (char *)bytes4 + 10, nparsed);
    ASSERT(res == server_template::base::ParseResult::INCOMPLETE)
    ASSERT(frame->header.fin)
    ASSERT(!frame->header.mask)
    ASSERT(frame->header.opcode == WebsocketOpcode::BINARY_FRAME)
    ASSERT(frame->getPayloadLength() == 65536)

    // bytes5, "Hello", no mask, with RSV1 on
    parser = WebsocketFrameParser();
    frame = parser.getFrame();
    res = parser.parse((char *)bytes5, (char *)bytes5 + 7, nparsed);
    ASSERT(res == server_template::base::ParseResult::COMPLETE)
    ASSERT(frame->header.fin)
    ASSERT(!frame->header.mask)
    ASSERT(frame->header.opcode == WebsocketOpcode::TEXT_FRAME)
    ASSERT(frame->getPayloadLength() == 5)
    ASSERT(frame->payload.equals("Hello"))
    ASSERT(frame->header.rsv1)
}