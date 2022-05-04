#ifndef SERVER_TEMPLATE_UTIL_BUILDER_H_
#define SERVER_TEMPLATE_UTIL_BUILDER_H_

#include "util_ns.h"

SERVER_TEMPLATE_UTIL_NAMESPACE_BEGIN

template <typename T>
class Builder
{
public:
    virtual ~Builder() {}

    virtual T *build() const = 0;
};

SERVER_TEMPLATE_UTIL_NAMESPACE_END

#endif // !SERVER_TEMPLATE_UTIL_BUILDER_H_