#ifndef SERVER_TEMPLATE_HTTP_HTTP_UPGRADE_PROTOCOL_H_
#define SERVER_TEMPLATE_HTTP_HTTP_UPGRADE_PROTOCOL_H_

#include "http_request.h"
#include "../tcp/tcp_based_protocol.h"
#include <functional>

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

class HttpUpgradeProtocol : public tcp::TCPBasedProtocol
{
public:
    using ProtocolFactory = std::function<HttpUpgradeProtocol *(base::ConfigurationBase *, tcp::ConnectionHandlerBase *)>;

    virtual void handleUpgrade(HttpRequest *req) = 0;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_UPGRADE_PROTOCOL_H_