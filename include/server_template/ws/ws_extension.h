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

    WebsocketExtension(int rsvIndex, const std::string &name, Type type)
    {
        this->rsvIndex = rsvIndex;
        this->name = name;
        this->type = type;
    }

    int getRsvIndex() const
    {
        return this->rsvIndex;
    }

    const std::string &getName() const
    {
        return this->name;
    }

    Type getType() const
    {
        return this->type;
    }

private:
    int rsvIndex;
    std::string name;
    Type type;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_EXTENSION_H_