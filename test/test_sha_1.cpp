#include <server_template/util/sha_1.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

#define MESSAGE "hello SHA-1"
#define HASH_RES "A591F0664949D5769E390D1BB49332174B556F1C"
#define EMPTY_HASH_RES "da39a3ee5e6b4b0d3255bfef95601890afd80709"
#define MESSAGE_2 "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

using ByteArray = server_template::util::ByteArray;
using SHA1 = server_template::util::SHA1;
using ByteFormat = server_template::util::ByteFormat;

int main()
{
    ByteArray bytes;
    SHA1::computeHash(ByteArray(MESSAGE, 11), bytes);
    std::string hexString;
    bytes.toString(hexString);
    ASSERT(strcmp(hexString.c_str(), HASH_RES) == 0)

    SHA1::computeHash(ByteArray(), bytes);
    bytes.toString(hexString, ByteFormat::HEX_LOWER);
    ASSERT(strcmp(hexString.c_str(), EMPTY_HASH_RES) == 0)

    SHA1::computeHash(ByteArray(MESSAGE_2, 60), bytes);
    bytes.toString(hexString);
    
}