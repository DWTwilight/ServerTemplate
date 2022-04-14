#include <server_template/util/byte_array.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

#define BYTES_1 "0123456\0abcefg"
#define BYTES_2 "0123456abcedf"
#define BYTES_3 "0123456\0abcefg0123456abcedf"
#define BYTES_4 "0123456abcedf0123456\0abcefg"
#define BYTES_5 "0123456abcedf0123456\0abcefga0123456\0abcefg0123456abcedf"

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
    auto array1 = ByteArray(BYTES_1, 14);
    ASSERT(checkBytes(array1.data(), (uint8_t*)BYTES_1, 14))

    auto array2 = ByteArray(BYTES_2);
    ASSERT(checkBytes(array2.data(), (uint8_t*)BYTES_2, 13))

    array1.append(BYTES_2);
    ASSERT(checkBytes(array1.data(), (uint8_t*)BYTES_3, 27))

    array2.append(BYTES_1, 14);
    ASSERT(checkBytes(array2.data(), (uint8_t*)BYTES_4, 27))

    array2.push_back('a');
    array2.append(array1);
    ASSERT(checkBytes(array2.data(), (uint8_t*)BYTES_5, 55))
}