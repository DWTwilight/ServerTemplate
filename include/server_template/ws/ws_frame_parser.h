#ifndef SERVER_TEMPLATE_WS_WS_PARSER_H_
#define SERVER_TEMPLATE_WS_WS_PARSER_H_

#include "../base/parser_base.h"
#include "ws_frame.h"
#include "../util/byte_converter.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketFrameParser : public base::ParserTemplate<WebsocketFrame>
{
public:
    WebsocketFrameParser() {}

    WebsocketFrameParser(size_t maxPayloadLength)
    {
        this->maxPayloadLength = maxPayloadLength;
    }

    virtual base::ParseResult parse(const char *begin, const char *end, size_t &nparsed) override
    {
        if (this->result == base::ParseResult::PARSE_ERROR)
        {
            return base::ParseResult::PARSE_ERROR;
        }
        nparsed = 0;
        this->result = consume(begin, end, nparsed);
        return this->result;
    }

    void setMaxPayloadLength(size_t value)
    {
        this->maxPayloadLength = value;
    }

private:
    base::ParseResult consume(const char *begin, const char *end, size_t &nparsed)
    {
        while (begin != end)
        {
            char input = *begin++;
            nparsed++;

            switch (this->status)
            {
            case ParserStatus::HEADER:
                if (headerBytes-- > 0)
                {
                    buffer.push_back(input);
                    if (headerBytes == 0)
                    {
                        // construct header
                        memcpy(&(frame.header), buffer.data(), 2U);
                        buffer.clear();
                        if (this->maxPayloadLength < frame.header.payloadLength)
                        {
                            // message is too large, abort
                            return base::ParseResult::PARSE_ERROR;
                        }

                        // cal length bytes
                        if (frame.header.payloadLength == 0)
                        {
                            this->payloadLength = 0;
                            if (frame.header.mask)
                            {
                                this->status = ParserStatus::MASK_KEY;
                            }
                            else
                            {
                                return base::ParseResult::COMPLETE;
                            }
                        }
                        else if (frame.header.payloadLength <= 125)
                        {
                            this->payloadLength = frame.header.payloadLength;
                            frame.payload.reserve(this->payloadLength);
                            this->status = frame.header.mask ? ParserStatus::MASK_KEY : ParserStatus::PAYLOAD;
                        }
                        else
                        {
                            this->payloadLengthBytes = frame.header.payloadLength == 126 ? 2 : 8;
                            this->status = ParserStatus::PAYLOAD_LENGTH;
                        }
                    }
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParserStatus::PAYLOAD_LENGTH:
                if (payloadLengthBytes-- > 0)
                {
                    buffer.push_back(input);
                    if (payloadLengthBytes == 0)
                    {
                        if (buffer.size() == 2)
                        {
                            // 2 bytes
                            uint16_t length;
                            util::ByteConverter::fromBytes(buffer.data(), length);
                            if (this->maxPayloadLength < length || length < 126)
                            {
                                // message is too large, abort
                                return base::ParseResult::PARSE_ERROR;
                            }
                            this->payloadLength = length;
                        }
                        else
                        {
                            // 8 bytes
                            uint64_t length;
                            util::ByteConverter::fromBytes(buffer.data(), length);
                            if (this->maxPayloadLength < length || length < UINT16_MAX)
                            {
                                // message is too large, abort
                                return base::ParseResult::PARSE_ERROR;
                            }
                            this->payloadLength = length;
                        }
                        buffer.clear();
                        frame.payload.reserve(this->payloadLength);
                        this->status = frame.header.mask ? ParserStatus::MASK_KEY : ParserStatus::PAYLOAD;
                    }
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParserStatus::MASK_KEY:
                if (maskKeyBytes-- > 0)
                {
                    frame.maskKey.push_back(input);
                    if (maskKeyBytes == 0)
                    {
                        if (this->payloadLength > 0)
                        {
                            this->status = ParserStatus::PAYLOAD;
                        }
                        else
                        {
                            return base::ParseResult::COMPLETE;
                        }
                    }
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParserStatus::PAYLOAD:
                if (payloadLength-- > 0)
                {
                    frame.payload.push_back(input);
                    if (payloadLength == 0)
                    {
                        // check if need to decode
                        if (frame.header.mask)
                        {
                            // decode
                            auto length = frame.getPayloadLength();
                            for (int i = 0; i < length; i++)
                            {
                                frame.payload[i] ^= frame.maskKey[i % 4];
                            }
                        }
                        return base::ParseResult::COMPLETE;
                    }
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            }
        }

        return base::ParseResult::INCOMPLETE;
    }

    enum class ParserStatus
    {
        HEADER,
        PAYLOAD_LENGTH,
        MASK_KEY,
        PAYLOAD
    };

    util::ByteArray buffer = util::ByteArray(8); // not for payload
    ParserStatus status = ParserStatus::HEADER;
    int headerBytes = 2;
    int payloadLengthBytes = 0;
    int maskKeyBytes = 4;
    size_t payloadLength;
    size_t maxPayloadLength = UINT32_MAX;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_PARSER_H_