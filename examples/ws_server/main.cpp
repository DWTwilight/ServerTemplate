#include <server_template/ws/websocket.h>
#include <server_template/http/http.h>
#include <server_template/tcp/tcp_server_template.h>
#include "endpoints/echo_endpoint.h"

class Configuration : public server_template::base::ConfigurationBase,
                      public server_template::http::HttpConfig,
                      public server_template::ws::WebsocketConfig
{
public:
    virtual void configServer(server_template::base::ServerConfigurations *server) override
    {
        server->useIpAddress(server_template::util::IpAddress("0.0.0.0", 5001));
        server->setMaxConnectionCount(10000);
    }

    virtual void configHttp(server_template::http::HttpConfigurations *protocol) override
    {
    }

    virtual void configUpgradeFactoryBuilder(server_template::http::HttpUpgradeFactoryBuilder &builder) override
    {
        builder.addUpgrade<server_template::ws::Websocket>("websocket");
    }

    virtual void configWebsocket(server_template::ws::WebsocketConfigurations *protocol) override
    {
    }

    virtual void configEndpoint(server_template::ws::WebsocketEndpointManager &endpointManager) override
    {
        endpointManager.addEndpoint<EchoEndpoint>("echo");
    }

    virtual void configPME(server_template::ws::WebsocketPMEManager &pmeManager) override
    {
    }
};

int main()
{
    // server_template::logDebug = true;
    auto loop = uv_default_loop();

    server_template::tcp::TCPServerTemplate<server_template::http::Http> httpServer;
    httpServer.useConfigClass<Configuration>();
    httpServer.run(loop);

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);

    return 0;
}