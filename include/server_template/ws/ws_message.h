#ifndef SERVER_TEMPLATE_WS_WS_MESSAGE_H_
#define SERVER_TEMPLATE_WS_WS_MESSAGE_H_

#include "ws_frame.h"
#include "../util/utf8.h"
#include <map>

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketMessage
{
public:
    enum class Type
    {
        UNKNOWN = 0,
        TEXT = 1,
        BINARY = 2
    };

    size_t getPayloadLength() const
    {
        return payload.size();
    }

    const bool* getRsv() const
    {
        return this->rsv;
    }

    /**
     * @brief add a frame to this message
     *
     * @param frame frame to add
     * @return true : the message is valid
     * @return false : parse error
     */
    bool addFrame(const WebsocketFrame* frame)
    {
        if (this->complete)
        {
            // parse error
            return false;
        }
        if (this->type == Type::UNKNOWN)
        {
            // first frame
            this->type = (Type)frame->header.opcode;
            if (this->type == Type::UNKNOWN)
            {
                // type cannot be 0
                return false;
            }
            // copy payload
            this->payload = frame->payload;
            // get rsv
            this->rsv[0] = frame->header.rsv1;
            this->rsv[1] = frame->header.rsv2;
            this->rsv[2] = frame->header.rsv3;
        }
        else
        {
            if (frame->header.opcode != WebsocketOpcode::CONTINUATION_FRAME)
            {
                // opcode must be 0
                return false;
            }
            if (frame->header.rsv1 || frame->header.rsv2 || frame->header.rsv3)
            {
                // continuation frame must set rsv to 0
                return false;
            }
            // append payload
            this->payload.append(frame->payload);
        }
        // check if complete
        this->complete = frame->header.fin;
        this->frameCount++;

        if (this->complete && this->type == Type::TEXT)
        {
            // check if is utf-8 encoded
            if (!util::UTF8::isValidUTF8(this->payload))
            {
                return false;
            }
        }

        return true;
    }

    util::ByteArray payload;
    std::map<std::string, util::ByteArray> extensionDataMap;
    Type type = Type::UNKNOWN;
    bool complete = false;
    int frameCount = 0;
    bool rsv[3];
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_MESSAGE_H_