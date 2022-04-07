#include <server_template/http/http_response.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

#define CORRECT_RESPONSE "HTTP/1.1 200 OK\r\ncontent-length: 42\r\ncontent-type: text/plain\r\nhost: libuv\r\n\r\nabcdefghijklmnopqrstuvwxyz1234567890abcdef"

int main()
{
    auto response = server_template::http::HttpResponse();

    response.version = 1.1;
    response.status = server_template::http::HttpStatus::OK;
    response.headerMap["content-type"] = "text/plain";
    response.headerMap["host"] = "libuv";
    response.payload = std::string("abcdefghijklmnopqrstuv\0wxyz1234567890abcdef", 43);

    return 0;
}