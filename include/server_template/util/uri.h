#ifndef SERVER_TEMPLATE_UTIL_URI_H_
#define SERVER_TEMPLATE_UTIL_URI_H_

#include "string_util.h"
#include <unordered_map>

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

/*
 *  parse uri in format [/<path>;<parameters>?<query>#<fragment>] 
 */
class Uri
{
public:
    static void parseUri(Uri* uri, const std::string& uriStr)
    {
        auto paramsPos = uriStr.find_first_of(';');
        auto queryPos = uriStr.find_first_of('?', paramsPos == std::string::npos ? 0 : paramsPos);
        auto fragmentPos = uriStr.find_first_of('#', queryPos == std::string::npos ? (paramsPos == std::string::npos ? 0 : paramsPos) : queryPos);

        auto min = std::min(paramsPos, queryPos);
        min = std::min(min, fragmentPos);
        if (min == std::string::npos)
        {
            uri->path = uriStr.substr(1);
            return;
        }
        uri->path = uriStr.substr(1, min - 1);

        if (paramsPos != std::string::npos)
        {
            min = std::min(queryPos, fragmentPos);
            if (min == std::string::npos)
            {
                parseMap(uriStr.substr(paramsPos + 1), uri->params);
                return;
            }
            parseMap(uriStr.substr(paramsPos + 1, min - paramsPos - 1), uri->params);
        }
        if (queryPos != std::string::npos)
        {
            if (fragmentPos == std::string::npos)
            {
                parseMap(uriStr.substr(queryPos + 1), uri->query);
                return;
            }
            parseMap(uriStr.substr(queryPos + 1, fragmentPos - queryPos - 1), uri->query);
        }
        uri->fragment = uriStr.substr(fragmentPos + 1);
    }

    Uri(const std::string& uri)
    {
        parseUri(this, uri);
    }

    Uri() {}

    std::string path;
    std::unordered_map<std::string, std::string> params;
    std::unordered_map<std::string, std::string> query;
    std::string fragment;

private:
    static void parseMap(const std::string& str, std::unordered_map<std::string, std::string>& map)
    {
        auto keyValuePairs = StringUtil::split(str, '&', true);
        for (auto pair : keyValuePairs)
        {
            auto pos = pair.find_first_of('=');
            if (pos != std::string::npos)
            {
                map[pair.substr(0, pos)] = pair.substr(pos + 1);
            }
        }
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_URI_H_