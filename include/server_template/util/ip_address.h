#ifndef SERVER_TEMPLATE_UTIL_IP_ADDRESS_H_
#define SERVER_TEMPLATE_UTIL_IP_ADDRESS_H_

#include <uv.h>
#include "util_ns.h"
#include "string_util.h"

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

enum class IpAddressType
{
    IPV4,
    IPV6
};

class IpAddress
{
public:
    IpAddress(const std::string &address = "0.0.0.0", int port = 8080, IpAddressType type = IpAddressType::IPV4)
    {
        this->address = address;
        this->port = port;
        this->type = type;
    }

    int parseAddress(sockaddr_storage *addr)
    {
        if (this->type == IpAddressType::IPV4)
        {
            return uv_ip4_addr(this->address.c_str(), this->port, (sockaddr_in *)addr);
        }
        else
        {
            return uv_ip6_addr(this->address.c_str(), this->port, (sockaddr_in6 *)addr);
        }
    }

    int getPort() const
    {
        return this->port;
    }

    const std::string &getAddress() const
    {
        return this->address;
    }

    std::string getSocketAddress() const
    {
        return StringUtil::format("%s:%d", this->address.c_str(), this->port);
    }

private:
    std::string address;
    int port;
    IpAddressType type;
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif