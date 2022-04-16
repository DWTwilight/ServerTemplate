#ifndef SERVER_TEMPLATE_HTTP_HTTP_H_
#define SERVER_TEMPLATE_HTTP_HTTP_H_

#include "http_request_parser.h"
#include "http_config.h"
#include "../tcp/tcp_based_protocol.h"
#include "http_response.h"

SERVER_TEMPLATE_HTTP_NAMESPACE_BEGIN

/**
 * @brief This is not a complete implementation of http protocol;
 * only http-upgrade protocols are supported;
 * for other requests, it just returns 400 Bad Request
 */
class Http : public tcp::TCPBasedProtocol, public HttpConfigurations
{
public:
    virtual ~Http()
    {
        if (this->upgradeFactory != NULL)
        {
            delete this->upgradeFactory;
        }
    }

    virtual void onTCPData(ssize_t nread, char *bytes) override
    {
        log("on data");
        if (this->upgrade == NULL)
        {
            size_t nparsed;
            auto res = parser.parse(bytes, bytes + nread, nparsed);
            if (res == base::ParseResult::PARSE_ERROR)
            {
                // parse error, close connection
                this->connHandler->closeConnection();
                return;
            }
            if (res == base::ParseResult::COMPLETE)
            {
                log("on http");
                onHttpMessage();
                log("after http");
                if (nparsed < nread)
                {
                    // bytes not read completely
                    onTCPData(nread - nparsed, bytes + nparsed);
                }
            }
        }
        else
        {
            this->upgrade->onTCPData(nread, bytes);
        }
    }

    virtual void onTCPConnectionClose() override
    {
        log("conn close");
        if (this->upgrade != NULL)
        {
            upgrade->onTCPConnectionClose();
        }
    }

    virtual void useConfig(base::ConfigurationBase *config) override
    {
        this->config = config;
        auto httpConfig = dynamic_cast<HttpConfig *>(config);
        httpConfig->configHttp(this);
        this->upgradeFactory = httpConfig->getHttpUpgradeFactoryBuilder().build();
    }

    virtual void setMaxContentLength(uint64_t value) override
    {
        this->maxContentLength = value;
        this->parser.setMaxContentLength(value);
    }

private:
    void onHttpMessage()
    {
        auto req = parser.getFrame();

        if (req->isUpgrade())
        {
            onUpgrade(req);
        }
        else
        {
            returnBadRequest("Bad Request", "text/plain");
            this->connHandler->closeConnection();
            return;
        }

        // reset parser
        parser = HttpRequestParser(maxContentLength);
    }

    void onUpgrade(HttpRequest *req)
    {
        auto name = req->headerMap.getValueOrDefault(UPGRADE_HEADER, "");
        this->upgrade = upgradeFactory->getProtocol(name, this->config, this->connHandler);
        if (this->upgrade == NULL)
        {
            returnBadRequest("Upgrade not supported!", "text/plain");
            this->connHandler->closeConnection();
        }
        else
        {
            this->upgrade->handleUpgrade(req);
        }
    }

    void returnBadRequest(const std::string &message, const std::string &contentType)
    {
        HttpResponse response;
        HttpResponse::buildBadRequest(response, message, contentType);
        util::ByteArray bytes;
        response.toBytes(bytes);

        this->connHandler->writeBytes(bytes);
    }

    base::ConfigurationBase *config;
    uint64_t maxContentLength = UINT64_MAX;
    HttpUpgradeFactory *upgradeFactory = NULL;
    HttpUpgradeProtocol *upgrade = NULL;
    HttpRequestParser parser;
};

SERVER_TEMPLATE_HTTP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_HTTP_HTTP_H_