#ifndef SERVER_TEMPLATE_UTIL_BYTE_CONVERTER_H_
#define SERVER_TEMPLATE_UTIL_BYTE_CONVERTER_H_

#include "byte_array.h"
#include <algorithm>

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

#define IS_LITTLE_ENDIAN char (0x0001)

/**
 * @brief convert to bytes to/from BIG_ENDIAN style; 
 * only for primitive types;
 * struct is NOT supported
 */
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

    template <typename T>
    static void arrayToBytes(const std::vector<T>& data, ByteArray& bytes)
    {
        arrayToBytes(data.data(), data.size(), bytes);
    }

    template <typename T>
    static void arrayToBytes(const T* data, size_t count, ByteArray& bytes)
    {
        bytes = ByteArray(count * sizeof(T));
        for (int i = 0; i < count; i++)
        {
            bytes.append((char*)(data + i), sizeof(T), IS_LITTLE_ENDIAN);
        }
    }

    template <typename T>
    static bool arrayFromBytes(const uint8_t* bytes, size_t count, std::vector<T>& data)
    {
        if (count % sizeof(T) != 0)
        {
            return false;
        }
        data.clear();
        data.reserve(count / sizeof(T));
        for (size_t i = 0; i < count; i += sizeof(T))
        {
            T d;
            fromBytes(bytes + i, d);
            data.push_back(d);
        }
        return true;
    }

    template <typename T>
    static bool arrayFromBytes(const uint8_t* bytes, size_t count, T* data)
    {
        if (count % sizeof(T) != 0)
        {
            return false;
        }
        for (size_t i = 0; i < count; i += sizeof(T))
        {
            fromBytes(bytes + i, data[i / sizeof(T)]);
        }
        return true;
    }

    template <typename T>
    static bool arrayFromBytes(ByteArray& bytes, std::vector<T>& data)
    {
        return arrayFromBytes(bytes.data(), bytes.size(), data);
    }

    template <typename T>
    static bool arrayFromBytes(ByteArray& bytes, T* data)
    {
        return arrayFromBytes(bytes.data(), bytes.size(), data);
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_BYTE_CONVERTER_H_