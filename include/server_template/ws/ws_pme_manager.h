#ifndef SERVER_TEMPLATE_WS_WS_PME_MANAGER_H_
#define SERVER_TEMPLATE_WS_WS_PME_MANAGER_H_

#include "ws_pme.h"
#include <unordered_map>

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

class WebsocketPMEManager
{
public:
    virtual ~WebsocketPMEManager()
    {
        for (auto &pair : extensionMap)
        {
            delete pair.second;
        }
    }

    template <typename Extension>
    WebsocketPMEManager *addPME()
    {
        auto extension = dynamic_cast<WebsocketPerMessageExtension *>(new Extension());
        if (extensionMap.find(extension->getName()) != extensionMap.end())
        {
            abort();
        }
        extensionMap[extension->getName()] = extension;
    }

    WebsocketPerMessageExtension *getPME(const std::string &name)
    {
        if (extensionMap.find(name) == extensionMap.end())
        {
            return NULL;
        }
        return extensionMap[name];
    }

private:
    std::unordered_map<std::string, WebsocketPerMessageExtension *> extensionMap;
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WS_PME_MANAGER_H_