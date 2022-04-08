#include <server_template/util/uri.h>
#define ASSERT(exp) \
    if (!(exp))     \
        return -1;

int main()
{
    auto uriStr = "/foo/foo/foo;param1=p1?query1=q1&query2=q2#frag";
    auto uri = server_template::util::Uri(uriStr);
    ASSERT(strcmp(uri.path.c_str(), "foo/foo/foo") == 0)
    ASSERT(uri.params.size() == 1)
    ASSERT(uri.query.size() == 2)
    ASSERT(strcmp(uri.fragment.c_str(), "frag") == 0)

    auto uriStr2 = "/foo/foo?query1=q1&query2=5";
    auto uri2 = server_template::util::Uri(uriStr2);
    ASSERT(uri2.query.size() == 2)
}