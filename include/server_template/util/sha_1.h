#ifndef SERVER_TEMPLATE_UTIL_SHA_1_H_
#define SERVER_TEMPLATE_UTIL_SHA_1_H_

#include "byte_array.h"

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

class SHA1
{
public:
    static void computeHash(const ByteArray& input, ByteArray& output)
    {
        auto data = input;
        output = ByteArray(20);
        // fill data
        auto fillCount = getFillCount(input.size());
        if (fillCount > 0)
        {
            data.push_back(0x80);
        }
        if (fillCount > 1)
        {
            data.append((uint8_t)0, fillCount - 1);
        }
        // append length
        

    }

private:
    static int getFillCount(int inputLength)
    {
        return 56 - (inputLength % 64);
    }

    
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_SHA_1_H_