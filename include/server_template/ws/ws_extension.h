#ifndef SERVER_TEMPLATE_WS_WS_EXTENSION_H_
#define SERVER_TEMPLATE_WS_WS_EXTENSION_H_

#include "ws_message.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketExtension
{
public:
    enum class Type
    {
        PER_MESSAGE,
        // maybe per frame in the futrue
    };

    WebsocketExtension(int rsvIndex, const std::string &name, const std::string &paramName, Type type)
    {
        this->rsvIndex = rsvIndex;
        this->name = name;
        this->paramName = paramName;
        this->type = type;
    }

    // /**
    //  * @brief parse param
    //  *
    //  * @param paramStr trimed str
    //  * @return true: success
    //  * @return false: parse error
    //  */
    // bool parseParam(const std::string &paramStr)
    // {
    //     auto pos = paramStr.find_first_of('=');
    //     if (pos == std::string::npos)
    //     {
    //         return false;
    //     }
    //     if (paramStr[pos + 1] == '\"')
    //     {
    //         // quoted
    //         if (paramStr[paramStr.length() - 1] != '\"')
    //         {
    //             // invalid quote
    //             return false;
    //         }
    //         param = paramStr.substr(pos + 2, paramStr.length() - pos - 3);
    //     }
    //     else
    //     {
    //         if (paramStr[paramStr.length() - 1] == '\"')
    //         {
    //             // invalid quote
    //             return false;
    //         }
    //         param = paramStr.substr(pos + 1, paramStr.length() - pos - 1);
    //     }
    // }

    int getRsvIndex() const
    {
        return this->rsvIndex;
    }

    const std::string &getName() const
    {
        return this->name;
    }

    const std::string &getParamName() const
    {
        return this->paramName;
    }

    Type getType() const
    {
        return this->type;
    }

private:
    int rsvIndex;
    std::string name;
    std::string paramName;
    Type type;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_EXTENSION_H_