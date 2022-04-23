#ifndef SERVER_TEMPLATE_WS_WEBSOCKET_H_
#define SERVER_TEMPLATE_WS_WEBSOCKET_H_

#include "ws_config.h"
#include "ws_message_parser.h"
#include "../http/http_upgrade_protocol.h"
#include "../util/base64.h"
#include "../util/sha_1.h"
#include <queue>

SERVER_TEMPLATE_WS_NAMESPACE_BEGIN

#define SEC_WEBSOCKET_KEY_HEADER "Sec-WebSocket-Key"
#define SEC_WEBSOCKET_VERSION_HEADER "Sec-WebSocket-Version"
#define SEC_WEBSOCKET_PROTOCOL_HEADER "Sec-WebSocket-Protocol"
#define SEC_WEBSOCKET_EXTENSIONS_HEADER "Sec-WebSocket-Extensions"
#define SEC_WEBSOCKET_ACCEPT_HEADER "Sec-WebSocket-Accept"
#define WEBSOCKET_CONST_STR "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

class Websocket : public http::HttpUpgradeProtocol, public WebsocketConfigurations, public WebsocketSessionHandler
{
public:
    enum class ConnectionStatus
    {
        Open,
        Closing,
        Closed
    };

    struct SendFrameTask
    {
        void *arg;
        WebsocketFrame *frame;
    };

    Websocket()
    {
    }

    virtual void handleUpgrade(http::HttpRequest *req, http::HttpResponse &response) override
    {
        // check req
        if (req->method != http::HttpMethod::GET)
        {
            http::HttpResponse::buildSimpleResponse(response, http::HttpStatus::METHOD_NOT_ALLOWED, "method must be GET", "text/plain");
            return;
        }
        if (req->majorVersion < 1 || req->minorVersion < 1)
        {
            http::HttpResponse::buildBadRequest(response, "HTTP version not supported", "text/plain");
            return;
        }
        if (!req->headerMap.hasHeaderKey(HOST_HEADER))
        {
            http::HttpResponse::buildBadRequest(response, "must have a HOST header", "text/plain");
            return;
        }
        // check session key
        auto encodedKey = req->headerMap.getValueOrDefault(SEC_WEBSOCKET_KEY_HEADER, "");
        util::ByteArray sessionKey;
        if (!util::Base64::decode(encodedKey, sessionKey) || sessionKey.size() != 16)
        {
            http::HttpResponse::buildBadRequest(response, "session key is invalid", "text/plain");
            return;
        }
        // check version
        auto versionHeader = req->headerMap.getValueOrDefault(SEC_WEBSOCKET_VERSION_HEADER, "");
        if (versionHeader.find("13") == std::string::npos)
        {
            http::HttpResponse::buildSimpleResponse(response, http::HttpStatus::UPGRADE_REQUIRED, "websocket version not supported", "text/plain");
            response.headerMap[SEC_WEBSOCKET_VERSION_HEADER] = "13";
            return;
        }
        // resource name
        this->endpoint = this->endpointManager->getEndpoint(req->uri.path);
        if (this->endpoint == NULL)
        {
            http::HttpResponse::buildSimpleResponse(response, http::HttpStatus::NOT_FOUND, "resource not found", "text/plain");
            return;
        }
        // pre authorize
        if (!this->endpoint->preAuthorize(req, this->sessionInfo.securityAttributes))
        {
            http::HttpResponse::buildSimpleResponse(response, http::HttpStatus::UNAUTHORIZED, "UNAUTHORIZED", "text/plain");
            return;
        }
        // subprotocol
        processSubprotocol(req->headerMap.getValueOrDefault(SEC_WEBSOCKET_PROTOCOL_HEADER, ""), this->sessionInfo.subprotocol);
        // extensions
        processExtensions(req->headerMap.getValueOrDefault(SEC_WEBSOCKET_EXTENSIONS_HEADER, ""), this->sessionInfo.pmeExtensions);
        // fill session info
        this->sessionInfo.sessionKey = encodedKey;
        this->sessionInfo.clientIpAddress = this->connHandler->getClientIpAddress();
        this->sessionInfo.handshakeRequest = *req;
        // construct response
        http::HttpResponse::buildSimpleResponse(response, http::HttpStatus::SWITCHING_PROTOCOLS);
        response.headerMap[UPGRADE_HEADER] = "websocket";
        response.headerMap[CONNECTION_HEADER] = "Upgrade";
        util::ByteArray bytes;
        encodedKey.append(WEBSOCKET_CONST_STR);
        log(encodedKey.c_str());
        util::SHA1::computeHash(util::ByteArray(encodedKey), bytes);
        bytes.toString(encodedKey);
        log(encodedKey.c_str());
        util::Base64::encode(bytes, response.headerMap[SEC_WEBSOCKET_ACCEPT_HEADER]);
        // subprotocol
        if (!this->sessionInfo.subprotocol.empty())
        {
            response.headerMap[SEC_WEBSOCKET_PROTOCOL_HEADER] = this->sessionInfo.subprotocol;
        }
        // extensions
        if (!this->sessionInfo.pmeExtensions.empty())
        {
            response.headerMap[SEC_WEBSOCKET_EXTENSIONS_HEADER] = this->sessionInfo.pmeExtensions[0].getOriginalToken();
            for (int i = 1; i < sessionInfo.pmeExtensions.size(); i++)
            {
                response.headerMap[SEC_WEBSOCKET_EXTENSIONS_HEADER].append(", ");
                response.headerMap[SEC_WEBSOCKET_EXTENSIONS_HEADER].append(this->sessionInfo.pmeExtensions[i].getOriginalToken());
            }
        }

        this->status = ConnectionStatus::Open;

        // frameQueue
        uv_sem_init(&this->frameQueueSem, 0);
        uv_rwlock_init(&this->frameQueueLock);

        // start writing thread
        auto workReq = new uv_work_t;
        workReq->data = this;
        uv_queue_work(
            this->connHandler->getTCPHandle()->loop, workReq,
            [](uv_work_t *req)
            {
                auto handler = (Websocket *)(req->data);
                handler->sendFramesAsync();
            },
            [](uv_work_t *req, int status)
            {
                delete req;
            });

        // conn open cb
        this->endpoint->onConnectionOpen(this);
    }

    virtual void onTCPData(ssize_t nread, char *bytes) override
    {
    }

    virtual void useConfig(base::ConfigurationBase *config) override
    {
        auto wsConfig = dynamic_cast<WebsocketConfig *>(config);
        wsConfig->initialize();
        wsConfig->configWebsocket(this);
        this->endpointManager = wsConfig->getEndpointManager();
        this->pmeManager = wsConfig->getPMEManager();
    }

    virtual void setMaxPayloadLength(size_t value) override
    {
        this->parser.setMaxPayloadLength(value);
    }

    virtual void closeConnection() override
    {
    }

    virtual void sendMessage(util::ByteArray &bytes, WebsocketMessage::Type type, bool fragmentation = false, size_t mtu = UINT16_MAX) override
    {
    }

    virtual const WebsocketSessionInfo *getSessionInfo() const override
    {
        return &this->sessionInfo;
    }

    void onControlFrame(WebsocketFrame *)
    {
    }

    void onMessage(WebsocketMessage *msg)
    {
        if (!this->sessionInfo.subprotocol.empty())
        {
            this->endpoint->onMessageSubprotocol(this->sessionInfo.subprotocol, this, msg);
        }
        else
        {
            this->endpoint->onMessage(this, msg);
        }
    }

    std::vector<WebsocketPMEInstance> *getPMEExtensions()
    {
        return &this->sessionInfo.pmeExtensions;
    }

    WebsocketMessageParser::Config *getParserConfig()
    {
        return &this->config;
    }

    void sendFramesAsync()
    {
        while (this->status == ConnectionStatus::Open)
        {
            // wait for a frame to send
            uv_sem_wait(&this->frameQueueSem);
            if (this->status != ConnectionStatus::Open)
            {
                return;
            }
            // aquire the frame to send
            uv_rwlock_wrlock(&this->frameQueueLock);
            if (this->status != ConnectionStatus::Open)
            {
                return;
            }
            auto frame = this->frameQueue.front();
            this->frameQueue.pop();
            uv_rwlock_wrunlock(&this->frameQueueLock);
            // send the frame
            auto asyncHandle = new uv_async_t;
            asyncHandle->data = new SendFrameTask{this, frame};
            uv_async_init(this->connHandler->getTCPHandle()->loop, asyncHandle,
                          [](uv_async_t *handle)
                          {
                              auto task = (SendFrameTask *)handle->data;
                              ((Websocket *)task->arg)->sendFrame(task->frame);
                              delete task->frame;
                              delete task;
                              delete handle;
                          });
            uv_async_send(asyncHandle);
        }
    }

    void sendFrame(WebsocketFrame *frame)
    {
        util::ByteArray bytes;
        frame->toBytes(bytes);
        this->connHandler->writeBytes(bytes);
    }

private:
    void processSubprotocol(const std::string &subprotocolHeader, std::string &subprotocol)
    {
        auto options = util::StringUtil::split(subprotocolHeader, ',', true);
        util::StringUtil::batchTrim(options);
        for (const auto &option : options)
        {
            if (this->endpoint->hasSubprotocol(option))
            {
                subprotocol = option;
                return;
            }
        }
    }

    void processExtensions(const std::string &extensionHeader, std::vector<WebsocketPMEInstance> &extensions)
    {
        auto options = util::StringUtil::split(extensionHeader, ',', true);
        util::StringUtil::batchTrim(options);
        for (const auto &option : options)
        {
            auto tokens = util::StringUtil::split(option, ';', true);
            util::StringUtil::batchTrim(tokens);
            // token[0] is extension name; others are params
            if (!tokens.empty())
            {
                auto pme = this->pmeManager->getPME(tokens[0]);
                if (pme != NULL && !this->rsv[pme->getRsvIndex()])
                {
                    auto instance = WebsocketPMEInstance(pme, option);
                    for (int i = 1; i < tokens.size(); i++)
                    {
                        auto params = util::StringUtil::split(tokens[i], "=\"", true);
                        if (!params.empty())
                        {
                            instance.addParam(params[0], params.size() == 2 ? params[1] : "");
                        }
                    }
                    extensions.push_back(instance);
                }
            }
        }
    }

    void sendControlFrame(WebsocketFrame &frame)
    {
        uv_rwlock_wrlock(&this->frameQueueLock);
        if (this->status != ConnectionStatus::Open)
        {
            return;
        }
        this->sendFrame(&frame);
        uv_rwlock_wrunlock(&this->frameQueueLock);
    }

    void sendMessageAsync()
    {
    }

    // Connection Status
    ConnectionStatus status = ConnectionStatus::Closed;

    // global config
    WebsocketEndpointManager *endpointManager;
    WebsocketPMEManager *pmeManager;

    // session info
    WebsocketSessionInfo sessionInfo;

    // endpoint
    WebsocketEndpoint *endpoint;

    // parser
    WebsocketMessageParser::Config config{this,
                                          [](void *arg, WebsocketFrame *frame)
                                          {
                                              ((Websocket *)arg)->onControlFrame(frame);
                                          },
                                          [](void *arg, WebsocketMessage *msg)
                                          {
                                              ((Websocket *)arg)->onMessage(msg);
                                          }};
    WebsocketMessageParser parser = WebsocketMessageParser(this->getPMEExtensions(), this->getParserConfig());

    // frameQueue
    std::queue<WebsocketFrame *> frameQueue; // frames to send
    uv_rwlock_t frameQueueLock;              // for queue op
    uv_sem_t frameQueueSem;                  // how many frames in the queue

    // rsv
    bool rsv[3] = {false, false, false};
};

SERVER_TEMPLATE_WS_NAMESPACE_END

#endif // !SERVER_TEMPLATE_WS_WEBSOCKET_H_