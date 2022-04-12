#ifndef SERVER_TEMPLATE_TCP_CONNECTION_HANDLER_BASE_H_
#define SERVER_TEMPLATE_TCP_CONNECTION_HANDLER_BASE_H_

#include "tcp_ns.h"
#include "../util/ip_address.h"
#include "../util/byte_array.h"

SERVER_TEMPLATE_TCP_NAMESPACE_BEGIN

class ConnectionHandlerBase
{
public:
    virtual void writeBytes(char *bytes, size_t length) = 0;

    void writeBytes(const util::ByteArray &bytes)
    {
        this->writeBytes((char *)bytes.data(), bytes.size());
    }

    virtual void closeConnection() = 0;

    virtual util::IpAddress *getClientIpAddress() = 0;
};

SERVER_TEMPLATE_TCP_NAMESPACE_END

#endif // !SERVER_TEMPLATE_TCP_CONNECTION_HANDLER_BASE_H_