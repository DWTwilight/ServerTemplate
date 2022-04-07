#ifndef SERVER_TEMPLATE_HTTP_HTTP_REQUEST_H_
#define SERVER_TEMPLATE_HTTP_HTTP_REQUEST_H_

#include "http_method.h"
#include "http_header_map.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

class HttpRequest
{
public:
    HttpMethod method;
    std::string uri;
    double version;
    HttpHeaderMap headerMap;
    util::ByteArray payload;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_REQUEST_H_