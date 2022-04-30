#include "echo_protocol/echo_protocol.h"

class Configuration : public server_template::base::ConfigurationBase, public EchoProtocolConfig
{
public:
    virtual void configServer(server_template::base::ServerConfigurations *configurations) override
    {
        configurations->useIpAddress(server_template::util::IpAddress("0.0.0.0", 8180));
        configurations->setMaxConnectionCount(3);
    }

    virtual void configEcho(EchoProtocolConfigurations *configurations) override
    {
        configurations->useEchoPrefix(" :: ");
    }
};

int main()
{
    server_template::logDebug = true;
    auto loop = uv_default_loop();

    server_template::tcp::TCPServerTemplate<EchoProtocol> echoServer;
    echoServer.useConfigClass<Configuration>();
    echoServer.run(loop);

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);

    return 0;
}
