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
        // copy data
        auto data = input;
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
        ByteConverter::toBytes(((uint64_t)input.size()) * 8, lengthBytes);
        data.append(lengthBytes);

        uint32_t buffer_h[5] = {0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u};
        auto blockCount = data.size() / 64;
        for (int i = 0; i < blockCount; i++)
        {
            uint32_t buffer_abcde[5];
            std::copy(buffer_h, buffer_h + 5, buffer_abcde);
            uint32_t buffer_w[80];
            uint32_t temp;

            // assign w0 ... w15
            ByteConverter::arrayFromBytes(data.data() + 64 * i, 64, buffer_w);
            // cal w16 ... w79
            for (int j = 16; j < 80; j++)
            {
                buffer_w[j] = circularLeftShift(buffer_w[j - 3] ^ buffer_w[j - 8] ^ buffer_w[j - 14] ^ buffer_w[j - 16], 1);
            }

            for (int j = 0; j < 80; j++)
            {
                temp = circularLeftShift(buffer_abcde[0], 5) +
                       func(buffer_abcde[1], buffer_abcde[2], buffer_abcde[3], j) +
                       buffer_abcde[4] +
                       buffer_w[j] +
                       k(j);
                buffer_abcde[4] = buffer_abcde[3];
                buffer_abcde[3] = buffer_abcde[2];
                buffer_abcde[2] = circularLeftShift(buffer_abcde[1], 30);
                buffer_abcde[1] = buffer_abcde[0];
                buffer_abcde[0] = temp;
            }
            
            for (int j = 0; j < 5; j++)
            {
                buffer_h[j] += buffer_abcde[j];
            }
        }
        ByteConverter::arrayToBytes(buffer_h, 5, output);
    }

private:
    static int getFillCount(size_t inputLength)
    {
        return (120 - (inputLength % 64)) % 64;
    }

    static uint32_t k(int t)
    {
        return SHA_1_K[t / 20];
    }

    static uint32_t func(uint32_t x, uint32_t y, uint32_t z, int t)
    {
        return SHA_1_FUNC[t / 20](x, y, z);
    }

    static uint32_t circularLeftShift(const uint32_t &x, int n)
    {
        return (x << n) | (x >> (32 - n));
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_SHA_1_H_