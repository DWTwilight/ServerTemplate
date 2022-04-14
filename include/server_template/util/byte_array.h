#ifndef SERVER_TEMPLATE_UTIL_BYTE_ARRAY_H_
#define SERVER_TEMPLATE_UTIL_BYTE_ARRAY_H_

#include "util_ns.h"
#include <string>
#include <vector>

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

class ByteArray : public std::vector<uint8_t>
{
public:
    ByteArray()
    {
        this->resize(0);
    }

    ByteArray(size_t size)
    {
        this->reserve(size);
    }

    ByteArray(const char *bytes, size_t length, bool reverse = false)
    {
        if (!reverse)
        {
            this->resize(length);
            std::copy(bytes, bytes + length, this->begin());
        }
        else
        {
            this->reserve(length);
            for (int i = 1; i <= length; i++)
            {
                this->push_back(bytes[length - i]);
            }
        }
    }

    ByteArray(const std::string &bytes)
    {
        this->resize(bytes.length());
        std::copy(bytes.begin(), bytes.end(), this->begin());
    }

    void append(const std::string &bytes)
    {
        this->reserve(this->size() + bytes.length());
        this->insert(this->end(), bytes.begin(), bytes.end());
    }

    void append(const char *bytes, size_t length)
    {
        this->reserve(this->size() + length);
        this->insert(this->end(), bytes, bytes + length);
    }

    void append(const ByteArray &bytes)
    {
        this->append((const char *)bytes.data(), bytes.size());
    }

    void append(uint8_t byte, size_t count)
    {
        this->reserve(this->size() + count);
        this->insert(this->end(), count, byte);
    }

    bool equals(const ByteArray &other)
    {
        if (other.size() != this->size())
        {
            return false;
        }
        for (int i = 0; i < this->size(); i++)
        {
            if (this->at(i) != other[i])
            {
                return false;
            }
        }
        return true;
    }

    bool equals(const std::string &other)
    {
        if (other.size() != this->size())
        {
            return false;
        }
        for (int i = 0; i < this->size(); i++)
        {
            if (this->at(i) != (uint8_t)other[i])
            {
                return false;
            }
        }
        return true;
    }

    bool equals(const uint8_t *other)
    {
        for (int i = 0; i < this->size(); i++)
        {
            if (this->at(i) != other[i])
            {
                return false;
            }
        }
        return true;
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_BYTE_ARRAY_H_