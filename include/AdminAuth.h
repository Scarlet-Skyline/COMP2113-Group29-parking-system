// AdminAuth.h
#ifndef ADMIN_AUTH_H
#define ADMIN_AUTH_H

#include <string>

namespace AdminAuth {
    //Comparison with ADMIN-PASSWORD in Constants. h
    bool verify(const std::string& input);
}

#endif