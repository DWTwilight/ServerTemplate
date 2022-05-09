#include <server_template/util/base64.h>
#include <cstring>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;
#define BYTES "0123456abcefg"
#define RESULT "MDEyMzQ1NmFiY2VmZw=="
#define EMPTY ""

using ByteArray = server_template::util::ByteArray;
using Base64 = server_template::util::Base64;

int main()
{
    auto bytes = ByteArray(BYTES);
    std::string encodeOuput;
    Base64::encode(bytes, encodeOuput);
    ASSERT(strcmp(encodeOuput.c_str(), RESULT) == 0)

    Base64::encode(ByteArray(), encodeOuput);
    ASSERT(strcmp(encodeOuput.c_str(), EMPTY) == 0)

    auto encodedString = std::string(RESULT);
    ByteArray decodeOuput;
    auto res = Base64::decode(encodedString, decodeOuput);
    ASSERT(res)
    ASSERT(decodeOuput.equals(bytes))

    res = Base64::decode(EMPTY, decodeOuput);
    ASSERT(res)
    ASSERT(decodeOuput.equals(EMPTY))
}