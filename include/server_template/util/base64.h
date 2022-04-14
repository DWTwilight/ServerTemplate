#ifndef SERVER_TEMPLATE_UTIL_BASE64_H_
#define SERVER_TEMPLATE_UTIL_BASE64_H_

#include "string_util.h"
#include "byte_array.h"
#include "util_ns.h"

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

const std::string BASE64_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

class Base64
{
public:
    static bool decode(const std::string &input, ByteArray &output)
    {
        if (input.empty())
        {
            output = ByteArray();
            return true;
        }
        int length = input.length();
        if (length % 4 != 0)
        {
            return false;
        }
        output = ByteArray(getDecodeLength(input));
        auto cap = output.capacity();
        int index = 0;
        while (index < input.length())
        {
            uint8_t bytes[4];
            for (int i = 0; i < 4; i++)
            {
                bytes[i] = getByte(input[index++]);
                if (bytes[i] == UINT8_MAX)
                {
                    return false;
                }
            }
            output.push_back((bytes[0] << 2) | (bytes[1] >> 4));
            if (output.size() == cap)
            {
                return true;
            }
            output.push_back((bytes[1] << 4) | (bytes[2]) >> 2);
            if (output.size() == cap)
            {
                return true;
            }
            output.push_back((bytes[2] << 6) | bytes[3]);
        }
        return true;
    }

    static void encode(const ByteArray &input, std::string &output)
    {
        output = std::string();
        if (input.empty())
        {
            return;
        }
        output.reserve(getEncodeLength(input));

        int length = input.size();
        int index = 0;
        while (index < length)
        {
            int eqCount = 2;
            uint8_t byte0 = input[index++], byte1 = 0, byte2 = 0;
            if (index < length)
            {
                byte1 = input[index++];
                eqCount--;
            }
            if (index < length)
            {
                byte2 = input[index++];
                eqCount--;
            }

            output.push_back(BASE64_ALPHABET[byte0 >> 2]);
            output.push_back(BASE64_ALPHABET[((byte0 & 0x03) << 4) | (byte1 >> 4)]);
            output.push_back(eqCount-- > 0 ? '=' : BASE64_ALPHABET[((byte1 & 0x0f) << 2) | (byte2 >> 6)]);
            output.push_back(eqCount-- > 0 ? '=' : BASE64_ALPHABET[byte2 & 0x3f]);
        }
    }

private:
    static size_t getDecodeLength(const std::string &input)
    {
        return input.length() / 4 * 3 - (input.length() - input.find_last_not_of('=') - 1);
    }

    static size_t getEncodeLength(const ByteArray &input)
    {
        return (input.size() / 3 + 1) * 4;
    }

    static uint8_t getByte(char base64Char)
    {
        if (StringUtil::isUpper(base64Char))
        {
            return base64Char - 'A';
        }
        if (StringUtil::isLower(base64Char))
        {
            return base64Char - 71;
        }
        if (StringUtil::isNumber(base64Char))
        {
            return base64Char + 4;
        }
        if (base64Char == '+')
        {
            return 62;
        }
        if (base64Char == '/')
        {
            return 63;
        }
        if (base64Char == '=')
        {
            return 0;
        }
        return UINT8_MAX;
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif