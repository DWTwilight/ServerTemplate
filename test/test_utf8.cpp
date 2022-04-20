#include <server_template/util/utf8.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

using UTF8 = server_template::util::UTF8;

int main()
{
    auto utf8String = "%u8FD9%u662F%u4E00%u6BB5UTF-8%u5B57%u7B26%u4E32"; // ASCII for 这是一段UTF-8字符串
    ASSERT(UTF8::isValidUTF8(utf8String))
}