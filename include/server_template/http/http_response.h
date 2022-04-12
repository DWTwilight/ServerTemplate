#ifndef SERVER_TEMPLATE_HTTP_HTTP_RESPONSE_H_
#define SERVER_TEMPLATE_HTTP_HTTP_RESPONSE_H_

#include "http_status.h"
#include "http_header_map.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

class HttpResponse
{
public:
    static void buildBadRequest(HttpResponse& response, const std::string& message, const std::string& contentType)
    {
        response.status = HttpStatus::BAD_REQUEST;
        response.majorVersion = 1;
        response.minorVersion = 1;
        response.headerMap[CONNECTION_HEADER] = "close";
        if (!message.empty())
        {
            response.payload = util::ByteArray(message);
            response.headerMap[CONTENT_TYPE_HEADER] = contentType;
        }
    }

    HttpStatus status;
    int majorVersion;
    int minorVersion;
    HttpHeaderMap headerMap;
    util::ByteArray payload;

    void toBytes(util::ByteArray& bytes)
    {
        // initial line
        bytes.append(util::StringUtil::format("HTTP/%d.%d %d %s\r\n",
                                                 this->majorVersion, this->minorVersion, (int)this->status, HttpStatusTool::getStatusString(this->status)));

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
        bytes.append("\r\n");

        // payload
        bytes.append(payload);
    }
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // ！SERVER_TEMPLATE_HTTP_HTTP_RESPONSE_H_