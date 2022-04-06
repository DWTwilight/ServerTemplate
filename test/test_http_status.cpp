#include <server_template/http/http_status.h>
#define ASSERT(exp) if(!(exp)) return -1;

int main()
{
    auto status = server_template::http::HttpStatus::ACCEPTED;
    auto statusStr = server_template::http::HttpStatusTool::getStatusString(status);

    ASSERT(strcmp(statusStr, "Accepted") == 0)
}