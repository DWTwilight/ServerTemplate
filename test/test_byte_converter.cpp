#include <server_template/util/byte_converter.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

using ByteArray = server_template::util::ByteArray;
using ByteConverter = server_template::util::ByteConverter;

int main()
{
    int32_t data1 = 8;
    ByteArray bytes;
    ByteConverter::toBytes(data1, bytes);
    ASSERT(bytes.equals((uint8_t *)"\0\0\0\010"))

    int32_t res1;
    ByteConverter::fromBytes(bytes, res1);
    ASSERT(res1 == data1)
}