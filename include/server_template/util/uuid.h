#ifndef SERVER_TEMPLATE_UTIL_UUID_H_
#define SERVER_TEMPLATE_UTIL_UUID_H_

#include <vector>
#include <iostream>
#include <sstream>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>
#include <string>
#include "util_ns.h"

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

class UUID
{
public:
    static unsigned char randomChar()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        return static_cast<unsigned char>(dis(gen));
    }

    static std::string generate(const unsigned int len)
    {
        std::stringstream ss;
        for (auto i = 0; i < len; i++)
        {
            auto rc = randomChar();
            std::stringstream hexstream;
            hexstream << std::hex << int(rc);
            auto hex = hexstream.str();
            ss << (hex.length() < 2 ? '0' + hex : hex);
        }
        return ss.str();
    }
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif