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

class TCPConnectionHandler : public uv_tcp_t, public ConnectionHandlerBase
{
public:
    static void allocBuffer(uv_handle_t *handle, size_t suggestedSize, uv_buf_t *buf)
    {
        buf->base = new char[suggestedSize];
        buf->len = suggestedSize;
    }

    TCPConnectionHandler(uv_pipe_t *pipe, const std::string &pipeName, base::ConfigurationBase *config,
                         TCPBasedProtocol::ProtocolFactory protocolFactory, uv_tcp_t *mainHandle)
    {
        this->serverPipe = pipe;
        this->pipeName = pipeName;
        this->config = config;
        this->protocolFactory = protocolFactory;
        this->mainHandle = mainHandle;
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
                     log("tcp handle close");
                     ((TCPConnectionHandler *)handle->data)->onConnectionClose();
                 });
    }

    virtual util::IpAddress *getClientIpAddress() override
    {
        return &this->clientIpAddress;
    }

    virtual const uv_tcp_s *getTCPHandle() const override
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
                }
            }
            else
            {
                uv_close((uv_handle_t *)tcpHandle, NULL);
            }
        }
        delete buf->base;
    }

    void closePipe()
    {
        log("close pipe");
        if (this->serverPipeFlag)
        {
            uv_close((uv_handle_t *)this->serverPipe->data,
                     [](uv_handle_t *handle)
                     {
                         delete handle;
                     });
            uv_close((uv_handle_t *)this->serverPipe,
                     [](uv_handle_t *handle)
                     {
                         delete handle;
                     });
        }
        if (this->pipeFlag)
        {
            uv_close((uv_handle_t *)this->pipe,
                     [](uv_handle_t *handle)
                     {
                         delete handle;
                     });
        }
        if (this->clientPipeFlag)
        {
            uv_close((uv_handle_t *)this->clientPipe,
                     [](uv_handle_t *handle)
                     {
                         delete handle;
                     });
        }

        uv_fs_t req;
        uv_fs_unlink(this->loop, &req, this->pipeName.c_str(), NULL);
    }

    uv_pipe_t *getServerPipe() const
    {
        return this->serverPipe;
    }

    const char *getPipeName() const
    {
        return this->pipeName.c_str();
    }

    uv_pipe_t *getClientPipe()
    {
        this->clientPipe = new uv_pipe_t;
        this->clientPipe->data = this->serverPipe->data;
        uv_pipe_init(this->loop, clientPipe, 1);
        this->clientPipeFlag = true;
        return this->clientPipe;
    }

    void start()
    {
        log("conn Start");
        uv_loop_t loop;
        this->loop = &loop;

        uv_loop_init(&loop);

        auto r = uv_pipe_init(&loop, this->serverPipe, 1);
        serverPipeFlag = true;
        log("server pipe inited");
        if (r == 0)
        {
            this->pipe = new uv_pipe_t;
            pipe->data = this;
            r = uv_pipe_init(&loop, pipe, 0);
            pipeFlag = true;
            log("client pipe inited");
            if (r == 0)
            {
                uv_fs_t req;
                uv_fs_unlink(this->loop, &req, this->pipeName.c_str(), NULL);
                r = uv_pipe_bind(pipe, this->pipeName.c_str());
                log("pipe binded");
                if (r == 0)
                {
                    r = uv_listen((uv_stream_t *)pipe, 2,
                                  [](uv_stream_t *pipe, int status)
                                  {
                                      log("pipe listening");
                                      auto handler = (TCPConnectionHandler *)(pipe->data);
                                      if (status == 0)
                                      {
                                          log("start accept pipe");
                                          auto clientPipe = handler->getClientPipe();
                                          auto r = uv_accept(pipe, (uv_stream_t *)clientPipe);
                                          if (r == 0)
                                          {
                                              log("pipe accepted");
                                              r = uv_read_start((uv_stream_t *)clientPipe, allocBuffer,
                                                                [](uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
                                                                {
                                                                    log("on read");
                                                                    auto client = (uv_tcp_t *)(stream->data);
                                                                    auto connHandler = (TCPConnectionHandler *)(client->data);
                                                                    connHandler->onPipeRead(stream, nread, buf);
                                                                    connHandler->closePipe();
                                                                });
                                              log("read start");
                                              if (r != 0)
                                              {
                                                  log("read error");
                                                  handler->closePipe();
                                              }
                                          }
                                          else
                                          {
                                              log(uv_strerror(r));
                                              handler->closePipe();
                                          }
                                      }
                                      else
                                      {
                                          handler->closePipe();
                                      }
                                  });
                    if (r == 0)
                    {
                        log("start to connect pipe");
                        auto conn = new uv_connect_t;
                        conn->data = serverPipe;
                        uv_pipe_connect(conn, serverPipe, this->getPipeName(),
                                        [](uv_connect_t *req, int status)
                                        {
                                            log("on pipe connected");
                                            auto pipe = (uv_pipe_t *)(req->data);
                                            auto client = (uv_tcp_t *)(pipe->data);
                                            if (status == 0)
                                            {
                                                // send the client over
                                                auto dummyBuf = uv_buf_init(".", 1);
                                                auto write = new uv_write_t;
                                                auto r = uv_write2(write, (uv_stream_t *)pipe, &dummyBuf, 1, (uv_stream_t *)client,
                                                                   [](uv_write_t *req, int status)
                                                                   {
                                                                       delete req;
                                                                   });
                                                log("handle sent");
                                            }
                                            delete req;
                                        });
                        log("pipe connected");
                    }
                    else
                    {
                        this->closePipe();
                    }
                }
                else
                {
                    this->closePipe();
                }
            }
            else
            {
                this->closePipe();
            }
        }
        else
        {
            this->closePipe();
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

        auto r = uv_tcp_getpeername(tcpHandle, (sockaddr *)&addr, (int *)&len);
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
    uv_pipe_t *clientPipe;
    base::ConfigurationBase *config;
    util::IpAddress clientIpAddress;
    uv_tcp_t *mainHandle;
    bool serverPipeFlag = false;
    bool clientPipeFlag = false;
    bool pipeFlag = false;

    TCPBasedProtocol::ProtocolFactory protocolFactory;
    TCPBasedProtocol *protocol = NULL;
};

SERVER_TEMPLATE_TCP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_TCP_TCP_CONNECTION_HANDLER_H_