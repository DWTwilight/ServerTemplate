#ifndef SERVER_TEMPLATE_HTTP_HTTP_RESPONSE_H_
#define SERVER_TEMPLATE_HTTP_HTTP_RESPONSE_H_

#include "http_status.h"
#include "http_header_map.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

class HttpResponse
{
public:
    HttpStatus status;
    double version;
    HttpHeaderMap headerMap;
    std::string payload;

    std::string toBytes()
    {
        // initial line
        auto response = util::StringUtil::format("HTTP/%.1lf %d %s",
                                                 this->version, (int)this->status, HttpStatusTool::getStatusString(this->status));
        response.append(CRLF);

        // headers
        if (payload.length() > 0)
        {
            headerMap[CONTENT_LENGTH_HEADER] = std::to_string(payload.length());
        }
        else
        {
            headerMap.removeValue(CONTENT_LENGTH_HEADER);
        }
        response.append(headerMap.toBytes());
        response.append(CRLF);

        // payload
        response.append(payload);

        return response;
    }
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // ！SERVER_TEMPLATE_HTTP_HTTP_RESPONSE_H_