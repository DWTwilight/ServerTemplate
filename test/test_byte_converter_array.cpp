#include <server_template/util/byte_converter.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

using ByteArray = server_template::util::ByteArray;
using ByteConverter = server_template::util::ByteConverter;

int main()
{
    std::vector<int32_t> intArray = {1, 2, 3};
    ByteArray bytes;
    ByteConverter::arrayToBytes(intArray, bytes);
    ASSERT(bytes.equals((uint8_t*)"\0\0\0\001\0\0\0\002\0\0\0\003"))

    std::vector<int32_t> res;
    ByteConverter::arrayFromBytes(bytes, res);
    ASSERT(res[1] == intArray[1])
}