#ifndef SERVER_TEMPLATE_WS_WS_MESSAGE_PARSER_H_
#define SERVER_TEMPLATE_WS_WS_MESSAGE_PARSER_H_

#include "ws_frame_parser.h"
#include "ws_pme.h"
#include <functional>

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketMessageParser : public base::ParserTemplate<WebsocketMessage>
{
public:
    struct Config
    {
        void *arg;
        std::function<void(void *, WebsocketFrame *)> onControlFrame;
        std::function<void(void *, WebsocketMessage *)> onMessage;
    };

    WebsocketMessageParser(std::vector<WebsocketPMEInstance> *extensions, Config *config, size_t maxPayloadLength = UINT32_MAX)
    {
        this->extensions = extensions;
        this->config = config;
        this->maxPayloadLength = maxPayloadLength;
        this->frameParser.setMaxPayloadLength(maxPayloadLength);
    }

    virtual base::ParseResult parse(const char *begin, const char *end, size_t &nparsed) override
    {
        nparsed = 0;
        this->result = consume(begin, end, nparsed);
        return this->result;
    }

    void setMaxPayloadLength(size_t value)
    {
        this->maxPayloadLength = value;
        this->frameParser.setMaxPayloadLength(maxPayloadLength);
    }

private:
    void reset()
    {
        this->frame = WebsocketMessage();
    }

    bool extensionDecode()
    {
        for (int i = this->extensions->size() - 1; i >= 0; i--)
        {
            auto &extension = this->extensions->at(i);
            if (this->frame.rsv[extension.getRsvIndex()])
            {
                // extension is on
                if (!extension.decodeMessage(this->frame))
                {
                    return false;
                }
            }
        }
        return true;
    }

    base::ParseResult consume(const char *begin, const char *end, size_t &nparsed)
    {
        size_t fn = 0;
        auto res = frameParser.parse(begin, end, fn);
        if (res == base::ParseResult::PARSE_ERROR)
        {
            return base::ParseResult::PARSE_ERROR;
        }
        if (res == base::ParseResult::COMPLETE)
        {
            nparsed += fn;
            if (frameParser.getFrame()->isControlFrame())
            {
                // control frame
                this->config->onControlFrame(this->config->arg, frameParser.getFrame());
            }
            else
            {
                auto frameRes = this->frame.addFrame(frameParser.getFrame());
                if (!frameRes)
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                // check if complete
                if (this->frame.complete)
                {
                    // extension decode
                    if (!extensionDecode())
                    {
                        return base::ParseResult::PARSE_ERROR;
                    }
                    this->config->onMessage(this->config->arg, &this->frame);
                    // reset message
                    this->reset();
                }
            }
            // reset frame parser
            frameParser = WebsocketFrameParser(maxPayloadLength);
            // check if parse complete
            if (end - begin > fn)
            {
                consume(begin + fn, end, nparsed);
            }
        }
        return base::ParseResult::INCOMPLETE;
    }

    WebsocketFrameParser frameParser;
    std::vector<WebsocketPMEInstance> *extensions; // in encode order
    Config *config;
    size_t maxPayloadLength = UINT32_MAX;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif