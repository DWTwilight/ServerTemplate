#ifndef SERVER_TEMPLATE_TCP_TCP_CONNECTION_HANDLER_H_
#define SERVER_TEMPLATE_TCP_TCP_CONNECTION_HANDLER_H_

#include <string>
#include "tcp_based_protocol.h"

SERVER_TEMPLATE_TCP_NAMESPACE_BEGIN

typedef struct
{
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

typedef struct
{
    uv_rwlock_t lock1;
    uv_rwlock_t lock2;
} double_rwlock_t;

class TCPConnectionHandler : public uv_tcp_t, public ConnectionHandlerBase
{
public:
    static void allocBuffer(uv_handle_t *handle, size_t suggestedSize, uv_buf_t *buf)
    {
        buf->base = new char[suggestedSize];
        buf->len = suggestedSize;
    }

    TCPConnectionHandler(uv_pipe_t *pipe, const std::string &pipeName, base::ConfigurationBase *config, TCPBasedProtocol::ProtocolFactory protocolFactory)
    {
        this->serverPipe = pipe;
        this->pipeName = pipeName;
        this->config = config;
        this->protocolFactory = protocolFactory;
    }

    virtual ~TCPConnectionHandler()
    {
        log("handler dtor called");
        if (this->protocol != NULL)
        {
            delete this->protocol;
        }
    }

    virtual void writeBytes(char *bytes, size_t length) override
    {
        auto writeReq = new uv_write_t;

        auto buffer = uv_buf_init(bytes, length);
        auto tcpHandle = (uv_tcp_s *)this;
        uv_write(writeReq, (uv_stream_t *)tcpHandle, &buffer, 1,
                 [](uv_write_t *req, int status)
                 {
                     delete req;
                 });
    }

    virtual void closeConnection() override
    {
        auto tcpHandle = (uv_tcp_s *)this;
        tcpHandle->data = this;
        uv_close((uv_handle_t *)tcpHandle,
                 [](uv_handle_t *handle)
                 {
                     ((TCPConnectionHandler *)handle->data)->onConnectionClose();
                 });
    }

    virtual util::IpAddress *getClientIpAddress() override
    {
        return &this->clientIpAddress;
    }

    virtual const uv_tcp_s* getTCPHandle() const override
    {
        return (uv_tcp_s *)this;
    }

    void onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
    {
        this->protocol->onTCPData(nread, buf->base);
    }

    void onConnectionClose()
    {
        this->protocol->onTCPConnectionClose();
    }

    void onPipeRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
    {
        if (nread > 0)
        {
            auto r = uv_tcp_init(this->loop, this);
            auto tcpHandle = (uv_tcp_s *)this;
            tcpHandle->data = this;
            if (r == 0)
            {
                r = uv_accept(stream, (uv_stream_t *)tcpHandle);
                if (r == 0)
                {
                    this->closePipe();
                    this->setClientIpAddress();
                    this->protocol = this->protocolFactory(this->config, this);
                    this->protocol->onTCPConnectionOpen();
                    log("conn open");
                    uv_read_start((uv_stream_t *)tcpHandle, allocBuffer,
                                  [](uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
                                  {
                                      if (nread > 0)
                                      {
                                          ((TCPConnectionHandler *)stream->data)->onRead(stream, nread, buf);
                                      }
                                      else
                                      {
                                          ((TCPConnectionHandler *)stream->data)->closeConnection();
                                      }
                                      delete buf->base;
                                  });
                }
                else
                {
                    uv_close((uv_handle_t *)tcpHandle, NULL);
                    this->closePipe();
                }
            }
            else
            {
                this->closePipe();
            }
        }
        delete buf->base;
    }

    void closePipe()
    {
        log("close pipe");
        uv_close((uv_handle_t *)this->serverPipe,
                 [](uv_handle_t *handle)
                 {
                     delete handle->data;
                     delete handle;
                 });
        uv_close((uv_handle_t *)this->pipe,
                 [](uv_handle_t *handle)
                 {
                     delete handle;
                 });
    }

    uv_pipe_t *getServerPipe() const
    {
        return this->serverPipe;
    }

    void start()
    {
        uv_loop_t loop;
        this->loop = &loop;

        uv_loop_init(&loop);

        auto r = uv_pipe_init(&loop, this->serverPipe, 1);
        if (r == 0)
        {
            this->pipe = new uv_pipe_t;
            pipe->data = this;
            r = uv_pipe_init(&loop, pipe, 0);
            if (r == 0)
            {
                r = uv_pipe_bind(pipe, this->pipeName.c_str());
                if (r == 0)
                {
                    auto doubleLock = (double_rwlock_t *)(this->data);
                    uv_rwlock_wrunlock(&(doubleLock->lock1));
                    uv_rwlock_wrlock(&(doubleLock->lock2));
                    r = uv_listen((uv_stream_t *)pipe, 128,
                                  [](uv_stream_t *pipe, int status)
                                  {
                                      auto handler = (TCPConnectionHandler *)(pipe->data);
                                      if (status == 0)
                                      {
                                          auto serverPipe = handler->getServerPipe();
                                          auto r = uv_accept(pipe, (uv_stream_t *)serverPipe);
                                          if (r == 0)
                                          {
                                              r = uv_read_start((uv_stream_t *)serverPipe, allocBuffer,
                                                                [](uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
                                                                {
                                                                    auto client = (uv_tcp_t *)(stream->data);
                                                                    auto connHandler = (TCPConnectionHandler *)(client->data);
                                                                    connHandler->onPipeRead(stream, nread, buf);
                                                                });
                                          }
                                          else
                                          {
                                              handler->closePipe();
                                          }
                                      }
                                      else
                                      {
                                          handler->closePipe();
                                      }
                                  });
                    if (r != 0)
                    {
                        this->closePipe();
                    }
                }
            }
        }

        uv_run(&loop, UV_RUN_DEFAULT);
        uv_loop_close(&loop);
        log("conn end");
    }

private:
    void setClientIpAddress()
    {
        auto tcpHandle = (uv_tcp_s *)this;
        sockaddr_storage addr;
        socklen_t len = sizeof(addr);
        char ipstr[47];
        int port;

        auto r = uv_tcp_getpeername(tcpHandle, (sockaddr *)&addr, &len);
        if (r != 0)
        {
            return;
        }

        if (addr.ss_family == AF_INET)
        {
            // ipv4
            sockaddr_in *addr_i4 = (sockaddr_in *)&addr;
            uv_ip4_name(addr_i4, ipstr, sizeof(ipstr));
            this->clientIpAddress = util::IpAddress(ipstr, ntohs(addr_i4->sin_port));
        }
        else
        {
            // ipv6
            sockaddr_in6 *addr_i6 = (sockaddr_in6 *)&addr;
            uv_ip6_name(addr_i6, ipstr, sizeof(ipstr));
            this->clientIpAddress = util::IpAddress(ipstr, ntohs(addr_i6->sin6_port), util::IpAddressType::IPV6);
        }
    }

    uv_pipe_t *serverPipe;
    std::string pipeName;
    uv_pipe_t *pipe;
    base::ConfigurationBase *config;
    util::IpAddress clientIpAddress;

    TCPBasedProtocol::ProtocolFactory protocolFactory;
    TCPBasedProtocol *protocol = NULL;
};

SERVER_TEMPLATE_TCP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_TCP_TCP_CONNECTION_HANDLER_H_