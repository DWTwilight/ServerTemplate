#include <server_template/http/http_status.h>

int main()
{
    auto status = server_template::http::HttpStatus::ACCEPTED;
    auto statusStr = server_template::http::HttpStatusTool::getStatusString(status);

    return strcmp(statusStr, "Accepted");
}