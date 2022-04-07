#include <server_template/http/http_response.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

#define CORRECT_RESPONSE "HTTP/1.1 200 OK\r\ncontent-length: 43\r\ncontent-type: text/plain\r\nhost: libuv\r\n\r\nabcdefghijklmnopqrstuv\0wxyz1234567890abcdef"

using ByteArray = server_template::util::ByteArray;

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

int main()
{
    auto response = server_template::http::HttpResponse();

    response.version = 1.1;
    response.status = server_template::http::HttpStatus::OK;
    response.headerMap["content-type"] = "text/plain";
    response.headerMap["host"] = "libuv";
    response.payload = ByteArray("abcdefghijklmnopqrstuv\0wxyz1234567890abcdef", 43);

    ByteArray bytes;
    response.toBytes(bytes);

    ASSERT(checkBytes(bytes.data(), (uint8_t*)CORRECT_RESPONSE, bytes.size()))
}