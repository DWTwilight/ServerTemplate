#ifndef SERVER_TEMPLATE_HTTP_HTTP_METHOD_H_
#define SERVER_TEMPLATE_HTTP_HTTP_METHOD_H_

#include "http_ns.h"
#include <string>

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

#undef DELETE
#define HTTP_METHOD_MAP(XX) \
    XX(0, DELETE)           \
    XX(1, GET)              \
    XX(2, HEAD)             \
    XX(3, POST)             \
    XX(4, PUT)              \
    XX(5, CONNECT)          \
    XX(6, OPTIONS)          \
    XX(7, TRACE)            \
    XX(8, COPY)             \
    XX(9, LOCK)             \
    XX(10, MKCOL)           \
    XX(11, MOVE)            \
    XX(12, PROPFIND)        \
    XX(13, PROPPATCH)       \
    XX(14, SEARCH)          \
    XX(15, UNLOCK)          \
    XX(16, BIND)            \
    XX(17, REBIND)          \
    XX(18, UNBIND)          \
    XX(19, ACL)             \
    XX(20, REPORT)          \
    XX(21, MKACTIVITY)      \
    XX(22, CHECKOUT)        \
    XX(23, MERGE)           \
    XX(24, MSEARCH)         \
    XX(25, NOTIFY)          \
    XX(26, SUBSCRIBE)       \
    XX(27, UNSUBSCRIBE)     \
    XX(28, PATCH)           \
    XX(29, PURGE)           \
    XX(30, MKCALENDAR)      \
    XX(31, LINK)            \
    XX(32, UNLINK)          \
    XX(33, SOURCE)          \
    XX(999, UNKNOWN)

enum class HttpMethod
{
#define XX(num, name) name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
};

class HttpMethodTool
{
public:
    static HttpMethod getMethodEnum(const char *method)
    {
#define XX(num, name)               \
    if (strcmp(#name, method) == 0) \
        return HttpMethod::name;
        HTTP_METHOD_MAP(XX)
#undef XX
        return HttpMethod::UNKNOWN;
    }

    static const char *getMethodString(const HttpMethod &method)
    {
        switch (method)
        {
#define XX(num, name)      \
    case HttpMethod::name: \
        return #name;
            HTTP_METHOD_MAP(XX)
#undef XX
        default:
            return "";
        }
    }
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_METHOD_H_