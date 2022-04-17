#ifndef SERVER_TEMPLATE_WS_WS_ENDPOINT_MANAGER_H_
#define SERVER_TEMPLATE_WS_WS_ENDPOINT_MANAGER_H_

#include "ws_endpoint.h"

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketEndpointManager
{
public:
    virtual ~WebsocketEndpointManager()
    {
        for (auto& pair : this->endpointMap)
        {
            delete pair.second;
        }
    }

    WebsocketEndpointManager* addEndpoint(const std::string& path, WebsocketEndpoint* endpoint)
    {
        if (this->endpointMap.find(path) != endpointMap.end())
        {
            abort();
        }
        this->endpointMap[path] = endpoint;
        return this;
    }

    WebsocketEndpoint* getEndpoint(const std::string& path)
    {
        if (this->endpointMap.find(path) != endpointMap.end())
        {
            return NULL;
        }
        return this->endpointMap[path];
    }

private:
    std::unordered_map<std::string, WebsocketEndpoint*> endpointMap;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_ENDPOINT_MANAGER_H_