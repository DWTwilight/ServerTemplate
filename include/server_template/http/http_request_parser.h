#ifndef SERVER_TEMPLATE_HTTP_HTTP_REQUEST_PARSER_H_
#define SERVER_TEMPLATE_HTTP_HTTP_REQUEST_PARSER_H_

#include "../base/parser_base.h"
#include "http_request.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

/**
 * @brief this is not a complete implemtation of http parser,
 * only supports normal http request like the ones for websocket handshake,
 * dose not support chunked encoding
 *
 */
class HttpRequestParser : public base::ParserTemplate<HttpRequest>
{
public:
    HttpRequestParser(uint64_t maxContentLength)
    {
        this->maxContentLength = maxContentLength;
    }

    virtual base::ParseResult parse(HttpRequest &frame, const char *begin, const char *end, size_t &nparsed) override
    {
        if (this->result == base::ParseResult::PARSE_ERROR)
        {
            return base::ParseResult::PARSE_ERROR;
        }
        nparsed = 0;
        this->result = consume(frame, begin, end, nparsed);
        return this->result;
    }

private:
    base::ParseResult consume(HttpRequest &frame, const char *begin, const char *end, size_t &nparsed)
    {
        while (begin != end)
        {
            char input = *begin++;
            nparsed++;

            switch (this->state)
            {
            case ParseState::METHOD:
                if (util::StringUtil::isUpper(input))
                {
                    buffer.push_back(input);
                }
                else if (input == ' ')
                {
                    frame.method = HttpMethodTool::getMethodEnum(this->buffer.c_str());
                    if (frame.method == HttpMethod::UNKNOWN)
                    {
                        return base::ParseResult::PARSE_ERROR;
                    }
                    // clear buffer and switch state
                    buffer.clear();
                    this->state = ParseState::URI;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::URI:
                if (util::StringUtil::isAlpha(input) || util::StringUtil::isNumber(input) || isUrlSpecial(input))
                {
                    buffer.push_back(input);
                }
                else if(input == ' ')
                {
                    util::Uri::parseUri(&frame.uri, this->buffer);
                    // clear buffer and switch state
                    buffer.clear();
                    this->state = ParseState::HTTP_SLASH;
                }
                break;
            }
        }

        return base::ParseResult::INCOMPLETE;
    }

    bool isUrlSpecial(char input)
    {
        switch (input)
        {
        case '-':
        case '.':
        case '_':
        case '~':
        case ':':
        case '/':
        case '?':
        case '#':
        case '[':
        case ']':
        case '@':
        case '!':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ';':
        case '%':
        case '=':
            return true;
        default:
            return false;
        }
    }

    enum class ParseState
    {
        METHOD,
        URI,
        HTTP_SLASH,
        VERSION,
        HEADER_KEY,
        HEADER_VALUE,
        NEW_LINE_LF,
        PAYLOAD
    };

    uint64_t maxContentLength;
    uint64_t contentLength;
    ParseState state = ParseState::METHOD;
    std::string buffer; // not for payload
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_REQUEST_PARSER_H_