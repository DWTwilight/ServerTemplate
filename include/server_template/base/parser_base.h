#ifndef SERVER_TEMPLATE_BASE_PARSER_BASE_H_
#define SERVER_TEMPLATE_BASE_PARSER_BASE_H_

#include "base_ns.h"

SERVER_TEMPLATE_BASE_NAMESPACE_BEGIN

enum class ParseResult
{
    COMPLETE,
    INCOMPLETE,
    PARSE_ERROR
};

template <typename FrameType>
class ParserTemplate
{
public:
    virtual ParseResult parse(const char *begin, const char *end, size_t &nparsed) = 0;

    ParseResult getResult() const
    {
        return this->result;
    }

    FrameType* getFrame()
    {
        return &frame;
    }

protected:
    ParseResult result;
    FrameType frame;
};

SERVER_TEMPLATE_BASE_NAMESPACE_END

#endif // !SERVER_TEMPLATE_BASE_PARSER_BASE_H_