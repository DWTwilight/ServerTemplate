#ifndef SERVER_TEMPLATE_HTTP_HTTP_REQUEST_H_
#define SERVER_TEMPLATE_HTTP_HTTP_REQUEST_H_

#include "http_method.h"
#include "http_header_map.h"
#include "../util/uri.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

class HttpRequest
{
public:
    HttpMethod method;
    util::Uri uri;
    int majorVersion;
    int minorVersion;
    HttpHeaderMap headerMap;
    util::ByteArray payload;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_REQUEST_H_