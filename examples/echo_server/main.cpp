#include "echo_protocol/echo_protocol.h"

class Configuration : public server_template::base::ConfigurationBase, public EchoProtocolConfig
{
public:
    virtual void configServer(server_template::base::ServerBase* server) override
    {
        server->useIpAddress(server_template::util::IpAddress("0.0.0.0", 8180));
    }

    virtual void configEcho(EchoProtocol *protocol) override
    {
        protocol->useEchoPrefix("server gets: ");
    }
};

int main()
{
    auto loop = uv_default_loop();

    server_template::tcp::TCPServerTemplate<EchoProtocol> echoServer;
    echoServer.useConfigClass<Configuration>();
    echoServer.run(loop);

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);

    return 0;
}
