#ifndef SERVER_TEMPLATE_UTIL_SHA_1_H_
#define SERVER_TEMPLATE_UTIL_SHA_1_H_

#include "byte_array.h"
#include "byte_converter.h"
#include <functional>

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

const uint32_t SHA_1_K[4] = {0x5A827999u, 0x6ED9EBA1u, 0x8F1BBCDCu, 0xCA62C1D6u};
const std::function<uint32_t(uint32_t, uint32_t, uint32_t)> SHA_1_FUNC[4] =
    {[](uint32_t x, uint32_t y, uint32_t z)
     {
         return (x & y) | (~x & z);
     },
     [](uint32_t x, uint32_t y, uint32_t z)
     {
         return x ^ y ^ z;
     },
     [](uint32_t x, uint32_t y, uint32_t z)
     {
         return (x & y) | (x & z) | (y & z);
     },
     [](uint32_t x, uint32_t y, uint32_t z)
     {
         return x ^ y ^ z;
     }};

class SHA1
{
public:
    static void computeHash(const ByteArray &input, ByteArray &output)
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
        ByteArray lengthBytes;
        ByteConverter::toBytes(input.size(), lengthBytes);
        data.append(lengthBytes);

        uint32_t buffer_h[5] = {0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u};
        auto blockCount = data.size() / 64;
        for (int i = 0; i < blockCount; i++)
        {
            uint32_t buffer_abcde[5];
            std::copy(buffer_h, buffer_h + 5, buffer_abcde);
            uint32_t buffer_w[80];
            uint32_t temp;
        }
    }

private:
    static int getFillCount(size_t inputLength)
    {
        return 56 - (inputLength % 64);
    }

    static uint32_t k(int t)
    {
        return SHA_1_K[t / 20];
    }

    static uint32_t func(uint32_t x, uint32_t y, uint32_t z, int t)
    {
        return SHA_1_FUNC[t / 20](x, y, z);
    }

    static uint32_t circularShift(const uint32_t &x, int n)
    {
        return (x << n) | (x >> (32 - n));
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_SHA_1_H_