#ifndef SERVER_TEMPLATE_UTIL_UTF8_H_
#define SERVER_TEMPLATE_UTIL_UTF8_H_

#include "byte_array.h"
#include <string>

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

class UTF8
{
public:
    static bool isValidUTF8(const ByteArray& bytes)
    {
        return isValidUTF8((char *)bytes.data());
    }

    static bool isValidUTF8(const std::string& string)
    {
        return isValidUTF8(string.c_str());
    }

    static bool isValidUTF8(const char *string)
    {
        if (!string)
            return true;

        const unsigned char *bytes = (const unsigned char *)string;
        unsigned int cp;
        int num;

        while (*bytes != 0x00)
        {
            if ((*bytes & 0x80) == 0x00)
            {
                // U+0000 to U+007F
                cp = (*bytes & 0x7F);
                num = 1;
            }
            else if ((*bytes & 0xE0) == 0xC0)
            {
                // U+0080 to U+07FF
                cp = (*bytes & 0x1F);
                num = 2;
            }
            else if ((*bytes & 0xF0) == 0xE0)
            {
                // U+0800 to U+FFFF
                cp = (*bytes & 0x0F);
                num = 3;
            }
            else if ((*bytes & 0xF8) == 0xF0)
            {
                // U+10000 to U+10FFFF
                cp = (*bytes & 0x07);
                num = 4;
            }
            else
                return false;

            bytes += 1;
            for (int i = 1; i < num; ++i)
            {
                if ((*bytes & 0xC0) != 0x80)
                    return false;
                cp = (cp << 6) | (*bytes & 0x3F);
                bytes += 1;
            }

            if ((cp > 0x10FFFF) ||
                ((cp >= 0xD800) && (cp <= 0xDFFF)) ||
                ((cp <= 0x007F) && (num != 1)) ||
                ((cp >= 0x0080) && (cp <= 0x07FF) && (num != 2)) ||
                ((cp >= 0x0800) && (cp <= 0xFFFF) && (num != 3)) ||
                ((cp >= 0x10000) && (cp <= 0x1FFFFF) && (num != 4)))
                return false;
        }

        return true;
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif