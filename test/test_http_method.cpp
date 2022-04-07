#include <server_template/http/http_method.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

int main()
{
    auto method = "SEARCH";
    auto methodEnum = server_template::http::HttpMethodTool::getMethodEnum(method);
    auto methodString = server_template::http::HttpMethodTool::getMethodString(methodEnum);

    ASSERT(methodEnum == server_template::http::HttpMethod::SEARCH)
    ASSERT(strcmp(method, methodString) == 0)
}