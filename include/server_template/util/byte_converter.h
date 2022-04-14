#ifndef SERVER_TEMPLATE_UTIL_BYTE_CONVERTER_H_
#define SERVER_TEMPLATE_UTIL_BYTE_CONVERTER_H_

#include "byte_array.h"

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

#define IS_LITTLE_ENDIAN char (0x0001)

class ByteConverter
{
public:
    template <typename T>
    static void toBytes(const T &data, ByteArray &bytes)
    {
        bytes = ByteArray((char*)&data, sizeof(T), IS_LITTLE_ENDIAN);
    }

    template <typename T>
    static bool fromBytes(const ByteArray& bytes, T& data)
    {
        if (bytes.size() != sizeof(T))
        {
            return false;
        }
        if (IS_LITTLE_ENDIAN)
        {
            auto bytesCopy = bytes;
            std::reverse(bytesCopy.begin(), bytesCopy.end());
            data = *((T*)(bytesCopy.data()));
            return true;
        }
        data = *((T*)(bytes.data()));
        return true;
    }

    template <typename T>
    static bool fromBytes(const uint8_t* bytes, T& data)
    {
        if (IS_LITTLE_ENDIAN)
        {
            auto bytesCopy = new uint8_t[sizeof(T)];
            for (int i = 0; i < sizeof(T); i++)
            {
                bytesCopy[i] = bytes[sizeof(T) - 1 - i];
            }
            data = *((T*)bytesCopy);
            delete bytesCopy;
            return true;
        }
        data = *((T*)(bytes));
        return true;
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_BYTE_CONVERTER_H_