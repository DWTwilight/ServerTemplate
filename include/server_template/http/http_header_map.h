#ifndef SERVER_TEMPLATE_HTTP_HTTP_HEADER_MAP_H_
#define SERVER_TEMPLATE_HTTP_HTTP_HEADER_MAP_H_

#include "http_ns.h"
#include "../util/string_util.h"
#include <unordered_map>
#include "../util/byte_array.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

#define CONTENT_LENGTH_HEADER "content-length"
#define CONNECTION_HEADER "connection"
#define UPGRADE_HEADER "Upgrade"
#define CONTENT_TYPE_HEADER "content-type"
#define HOST_HEADER "host"

class HttpHeaderMap
{
public:
    std::string &operator[](const std::string &key)
    {
        std::string headerKey;
        util::StringUtil::toLower(key, headerKey);
        return map[headerKey];
    }

    void removeValue(const std::string &key)
    {
        std::string headerKey;
        util::StringUtil::toLower(key, headerKey);
        if (map.find(headerKey) != map.end())
        {
            map.erase(headerKey);
        }
    }

    std::string getValueOrDefault(const std::string &key, const std::string &defaultValue)
    {
        std::string headerKey;
        util::StringUtil::toLower(key, headerKey);
        if (map.find(headerKey) == map.end())
        {
            return defaultValue;
        }
        return map[headerKey];
    }

    bool hasHeaderKey(const std::string &key) const
    {
        std::string headerKey;
        util::StringUtil::toLower(key, headerKey);
        return map.find(headerKey) != map.end();
    }

    void toBytes(util::ByteArray &bytes) const
    {
        for (auto pair : map)
        {
            bytes.append(util::StringUtil::format("%s: %s", pair.first.c_str(), pair.second.c_str()));
            bytes.append("\r\n");
        }
    }

    int count() const
    {
        return this->map.size();
    }

private:
    std::unordered_map<std::string, std::string> map;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // ！SERVER_TEMPLATE_HTTP_HTTP_HEADER_MAP_H_