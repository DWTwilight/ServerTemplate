#ifndef SERVER_TEMPLATE_TCP_TCP_SERVER_TEMPLATE_H_
#define SERVER_TEMPLATE_TCP_TCP_SERVER_TEMPLATE_H_

#include "tcp_ns.h"
#include "../base/server_base.h"
#include "../util/uuid.h"
#include "tcp_connection_handler.h"
#include <assert.h>

SERVER_TEMPLATE_TCP_NAMESPACE_BEGIN

#ifdef _WIN32
#define HANDLE_PIPE_NAME_PREFIX "\\\\?\\pipe\\hp-"
#else
#define HANDLE_PIPE_NAME_PREFIX "/tmp/hp-"
#endif

template <typename Protocol>
class TCPServerTemplate : public uv_tcp_t, public base::ServerBase
{
public:
    virtual ~TCPServerTemplate() {}

    virtual void run(uv_loop_t *loop) override
    {
        this->loadConfig();
        this->loop = loop;
        assert(this->loop != NULL);
        // parse address
        sockaddr_storage addr;

        if (this->ipAddress.parseAddress(&addr) < 0)
        {
            abort();
        }

        auto r = uv_tcp_init(this->loop, this);
        if (r != 0)
        {
            abort();
        }

        r = uv_tcp_bind(this, (sockaddr *)&addr, 0);
        if (r != 0)
        {
            abort();
        }
        assert(this->maxConnectionCount > 0);

        // Start listen
        auto tcpHandle = (uv_tcp_s *)this;
        tcpHandle->data = this;
        r = uv_listen((uv_stream_t *)tcpHandle, this->maxConnectionCount,
                      [](uv_stream_t *server, int status)
                      {
                          if (status < 0)
                          {
                              return;
                          }
                          ((TCPServerTemplate<Protocol> *)server->data)->onConnection();
                      });

        if (r != 0)
        {
            abort();
        }
    }

    virtual void useIpAddress(const util::IpAddress &ipAddress) override
    {
        this->ipAddress = ipAddress;
    }

    virtual void setMaxConnectionCount(uint32_t value) override
    {
        this->maxConnectionCount = value;
    }

    void onConnection()
    {
        log("on connection");
        // Accept connection
        auto *client = new uv_tcp_t;
        auto r = uv_tcp_init(this->loop, client);
        if (r != 0)
        {
            uv_close((uv_handle_t *)client,
                     [](uv_handle_t *handle)
                     {
                         delete handle;
                     });
            return;
        }
        auto tcpHandle = (uv_tcp_s *)this;
        r = uv_accept((uv_stream_t *)tcpHandle, (uv_stream_t *)client);
        if (r == 0)
        {
            // create a pipe to send client handle
            auto pipe = new uv_pipe_t;
            // random pipe name
            auto pipeName = std::string(HANDLE_PIPE_NAME_PREFIX).append(util::UUID::generate(20)).append(std::to_string((size_t)pipe));

            // start a new thread to read
            auto connHandler = new TCPConnectionHandler(pipe, pipeName, this->config, this->protocolFactory, client);
            client->data = connHandler;
            pipe->data = client;

            auto workReq = new uv_work_t;
            workReq->data = connHandler;
            uv_queue_work(
                this->loop, workReq,
                [](uv_work_t *req)
                {
                    auto handler = (TCPConnectionHandler *)(req->data);
                    handler->start();
                    delete handler;
                },
                [](uv_work_t *req, int status)
                {
                    delete req;
                });
        }
        else
        {
            printf("error accpet\n");
            uv_close((uv_handle_t *)client,
                     [](uv_handle_t *handle)
                     {
                         delete handle;
                     });
        }
    }

private:
    void loadConfig()
    {
        assert(this->config != NULL);
        this->config->configServer(this);
    }

    util::IpAddress ipAddress;
    uint32_t maxConnectionCount = 512;
    TCPBasedProtocol::ProtocolFactory protocolFactory =
        [](base::ConfigurationBase *config, ConnectionHandlerBase *connHandler)
    {
        auto protocol = dynamic_cast<TCPBasedProtocol *>(new Protocol());
        protocol->setConnectionHandler(connHandler);
        protocol->useConfig(config);
        return protocol;
    };
};

SERVER_TEMPLATE_TCP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_TCP_TCP_SERVER_TEMPLATE_H_