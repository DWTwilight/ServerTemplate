#ifndef SERVER_TEMPLATE_HTTP_HTTP_REQUEST_PARSER_H_
#define SERVER_TEMPLATE_HTTP_HTTP_REQUEST_PARSER_H_

#include "../base/parser_base.h"
#include "http_request.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

/**
 * @brief this is not a complete implemtation of http parser,
 * only supports simple http request like the ones for websocket handshake,
 * chunked encoding is not supported
 * LF format is not supported
 */
class HttpRequestParser : public base::ParserTemplate<HttpRequest>
{
public:
    HttpRequestParser(uint64_t maxContentLength)
    {
        this->maxContentLength = maxContentLength;
    }

    HttpRequestParser() {}

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

private:
    base::ParseResult consume(const char *begin, const char *end, size_t &nparsed)
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
                else if (input == ' ')
                {
                    util::Uri::parseUri(&frame.uri, this->buffer);
                    // clear buffer and switch state
                    buffer.clear();
                    this->state = ParseState::HTTP_H;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::HTTP_H:
                if (input == 'H')
                {
                    this->state = ParseState::HTTP_HT;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::HTTP_HT:
                if (input == 'T')
                {
                    this->state = ParseState::HTTP_HTT;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::HTTP_HTT:
                if (input == 'T')
                {
                    this->state = ParseState::HTTP_HTTP;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::HTTP_HTTP:
                if (input == 'P')
                {
                    this->state = ParseState::HTTP_HTTP_SLASH;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::HTTP_HTTP_SLASH:
                if (input == '/')
                {
                    this->state = ParseState::MAJOR_VERSION;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::MAJOR_VERSION:
                if (util::StringUtil::isNumber(input))
                {
                    frame.majorVersion = input - '0';
                    this->state = ParseState::VERSION_DOT;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::VERSION_DOT:
                if (input == '.')
                {
                    this->state = ParseState::MINOR_VERSION;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::MINOR_VERSION:
                if (util::StringUtil::isNumber(input))
                {
                    frame.minorVersion = input - '0';
                    this->state = ParseState::NEW_LINE_CR;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::NEW_LINE_CR:
                if (input == CR)
                {
                    this->state = ParseState::NEW_LINE_CRLF;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::NEW_LINE_CRLF:
                if (input == LF)
                {
                    this->state = ParseState::HEADER_KEY;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::HEADER_KEY:
                if (util::StringUtil::isLWSP(input))
                {
                    this->state = ParseState::HEADER_VALUE;
                }
                else if (input == ':')
                {
                    // header key complete
                    // check if header empty?
                    if (this->buffer.empty())
                    {
                        return base::ParseResult::PARSE_ERROR;
                    }
                    // store header key in currentHeaderKey
                    this->currentHeaderKey = this->buffer;
                    // clear buffer & change state
                    buffer.clear();
                    this->state = ParseState::HEADER_VALUE;
                }
                else if (!util::StringUtil::isCtls(input))
                {
                    buffer.push_back(input);
                }
                else if (input == CR && this->buffer.empty())
                {
                    this->state = ParseState::NEW_LINE_CRLF_END;
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::HEADER_VALUE:
                if (util::StringUtil::isLWSP(input))
                {
                    if (!buffer.empty())
                    {
                        buffer.push_back(input);
                    }
                }
                else if (input == CR)
                {
                    this->state = ParseState::NEW_LINE_LF;
                }
                else
                {
                    buffer.push_back(input);
                }
                break;
            case ParseState::NEW_LINE_LF:
                if (input == LF)
                {
                    // end of header line
                    if (this->currentHeaderKey.empty())
                    {
                        return base::ParseResult::PARSE_ERROR;
                    }
                    if (frame.headerMap.hasHeaderKey(currentHeaderKey))
                    {
                        frame.headerMap[currentHeaderKey].append(buffer);
                    }
                    else
                    {
                        frame.headerMap[currentHeaderKey] = buffer;
                    }
                    // clear buffer & change state
                    buffer.clear();
                    this->state = ParseState::HEADER_KEY;
                }
                else
                {
                    buffer.push_back(CR);
                    buffer.push_back(input);
                    this->state = ParseState::HEADER_VALUE;
                }
                break;
            case ParseState::NEW_LINE_CRLF_END:
                if (input == LF)
                {
                    // end of header fields
                    // get content-length
                    this->contentLength = std::stoull(frame.headerMap.getValueOrDefault(CONTENT_LENGTH_HEADER, "0"));
                    if (this->contentLength == 0)
                    {
                        // parse compelte
                        return base::ParseResult::COMPLETE;
                    }
                    else if (this->maxContentLength < this->contentLength)
                    {
                        // message is too large, abort
                        return base::ParseResult::PARSE_ERROR;
                    }
                    else
                    {
                        frame.payload.reserve(this->contentLength);
                        this->state = ParseState::PAYLOAD;
                    }
                }
                else
                {
                    return base::ParseResult::PARSE_ERROR;
                }
                break;
            case ParseState::PAYLOAD:
                frame.payload.push_back(input);
                if (frame.payload.size() == this->contentLength)
                {
                    // parse complete
                    return base::ParseResult::COMPLETE;
                }
                break;
            default:
                return base::ParseResult::PARSE_ERROR;
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
        HTTP_H,
        HTTP_HT,
        HTTP_HTT,
        HTTP_HTTP,
        HTTP_HTTP_SLASH,
        MAJOR_VERSION,
        VERSION_DOT,
        MINOR_VERSION,
        HEADER_KEY,
        HEADER_VALUE,
        NEW_LINE_CR,
        NEW_LINE_LF,
        NEW_LINE_CRLF,
        NEW_LINE_CRLF_END,
        PAYLOAD
    };

    uint64_t maxContentLength = UINT64_MAX;
    uint64_t contentLength;
    ParseState state = ParseState::METHOD;
    std::string buffer; // not for payload
    std::string currentHeaderKey;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_REQUEST_PARSER_H_