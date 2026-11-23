// AdminAuth.cpp
#include "AdminAuth.h"
#include "Constants.h"

namespace AdminAuth
{

    bool verify(const std::string &input)
    {
        return input == ADMIN_PASSWORD;
    }

} // namespace AdminAuth