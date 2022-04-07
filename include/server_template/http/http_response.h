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
    util::ByteArray payload;

    void toBytes(util::ByteArray& bytes)
    {
        // initial line
        bytes.append(util::StringUtil::format("HTTP/%.1lf %d %s",
                                                 this->version, (int)this->status, HttpStatusTool::getStatusString(this->status)));
        bytes.append(CRLF);

        // headers
        if (payload.size() > 0)
        {
            headerMap[CONTENT_LENGTH_HEADER] = std::to_string(payload.size());
        }
        else
        {
            headerMap.removeValue(CONTENT_LENGTH_HEADER);
        }
        headerMap.toBytes(bytes);
        bytes.append(CRLF);

        // payload
        bytes.append(payload);
    }
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // ！SERVER_TEMPLATE_HTTP_HTTP_RESPONSE_H_