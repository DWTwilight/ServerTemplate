#include <server_template/util/byte_converter.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

using ByteArray = server_template::util::ByteArray;
using ByteConverter = server_template::util::ByteConverter;

struct MyStruct
{
    int32_t val1;
    uint64_t val2;
    char val3;
};

int main()
{
    int32_t data1 = 8;
    ByteArray bytes;
    ByteConverter::toBytes(data1, bytes);
    ASSERT(bytes.equals((uint8_t *)"\0\0\0\010"))

    int32_t res1;
    ByteConverter::fromBytes(bytes, res1);
    ASSERT(res1 == data1)

    MyStruct data2 = {456, UINT64_MAX, '*'};
    ByteConverter::toBytes(data2, bytes);
    MyStruct res2;
    auto res = ByteConverter::fromBytes(bytes, res2);
    ASSERT(res)
    ASSERT(data2.val1 == res2.val1)
    ASSERT(data2.val2 == res2.val2)
    ASSERT(data2.val3 == res2.val3)

    res = ByteConverter::fromBytes(bytes.data(), res2);
    ASSERT(res)
    ASSERT(data2.val1 == res2.val1)
    ASSERT(data2.val2 == res2.val2)
    ASSERT(data2.val3 == res2.val3)
}