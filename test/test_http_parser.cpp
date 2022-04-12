#include <server_template/http/http_request_parser.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

bool checkBytes(uint8_t* array1, uint8_t* array2, size_t length)
{
    for (int i = 0; i < length; i++)
    {
        if (array1[i] != array2[i])
        {
            return false;
        }
    }
    return true;
}

const auto HTTP_REQ_1 = "GET /path/to/file/index.html HTTP/1.0\r\nFrom: someuser@jmarshall.com\r\nUser-Agent: HTTPTool/1.0\r\n\r\n";
const auto HTTP_REQ_2 = "POST /path/to/file/upload HTTP/1.0\r\nFrom: someuser@jmarshall.com\r\nUser-Agent: HTTPTool/1.0\r\nContent-Length: 5\r\n\r\n123\0z12345";
const auto HTTP_REQ_3_1 = "GET /path/to/file/index.html HTTP/1.0\r\nFrom: someuser@";
const auto HTTP_REQ_3_2 = "jmarshall.com\r\nUser-Agent: HTTPTool/1.0\r\n\r\n";

using HttpParser = server_template::http::HttpRequestParser;
using Request = server_template::http::HttpRequest;

int main()
{
    HttpParser parser;
    Request* req = parser.getFrame();
    size_t nparsed;

    auto status = parser.parse(HTTP_REQ_1, HTTP_REQ_1 + 97, nparsed);
    ASSERT(status == server_template::base::ParseResult::COMPLETE)
    ASSERT(req->payload.empty())
    ASSERT(req->headerMap.count() == 2)

    parser = HttpParser();
    req = parser.getFrame();
    status = parser.parse(HTTP_REQ_2, HTTP_REQ_2 + 123, nparsed);
    ASSERT(status == server_template::base::ParseResult::COMPLETE)
    ASSERT(checkBytes(req->payload.data(), (uint8_t*)"123\0z1", 5))
    ASSERT(req->headerMap.count() == 3)
    ASSERT(nparsed == 123 - 5)

    parser = HttpParser();
    req = parser.getFrame();
    status = parser.parse(HTTP_REQ_3_1, HTTP_REQ_3_1 + 54, nparsed);
    ASSERT(status == server_template::base::ParseResult::INCOMPLETE)
    status = parser.parse(HTTP_REQ_3_2, HTTP_REQ_3_2 + 43, nparsed);
    ASSERT(status == server_template::base::ParseResult::COMPLETE)
    ASSERT(req->payload.empty())
    ASSERT(req->headerMap.count() == 2)
}