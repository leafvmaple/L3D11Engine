#pragma once

#include <string>
#include <algorithm>

namespace L3D
{
    class lstring : public std::string
    {
    public:
        lstring() {}
        lstring(const char* szName) : std::string(szName)
        {
            std::transform(this->begin(), this->end(), this->begin(), tolower);
        }
    };
}
