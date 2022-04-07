#ifndef SERVER_TEMPLATE_HTTP_HTTP_PARSER_H_
#define SERVER_TEMPLATE_HTTP_HTTP_PARSER_H_

#include "../base/parser_base.h"
#include "http_request.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

class HttpParser : public base::ParserTemplate<HttpRequest>
{
public:
    virtual base::ParseResult parse(HttpRequest &frame, const char *begin, const char *end, size_t &nparsed) override
    {
    }
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_PARSER_H_