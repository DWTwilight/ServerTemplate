#include "server_template_ns.h"
#include <iostream>
#include <chrono>

bool server_template::logDebug = false;

void server_template::log(const char *msg)
{
    if (server_template::logDebug)
    {
        auto ts = std::chrono::system_clock::now();
        uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(ts.time_since_epoch()).count() % 1000;
        time_t t = time(NULL);
        tm *now = localtime(&t);

        printf("[%04d-%02d-%02d %02d:%02d:%02d.%04d] %s\n",
               now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, (int)dis_millseconds, msg);
    }
}