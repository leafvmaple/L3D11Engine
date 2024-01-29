#pragma once

#include <string>
#include <algorithm>
#include <filesystem>
#include <utility>

namespace L3D
{
    class lower_string : public std::string
    {
    public:
        lower_string() {}
        lower_string(const char* szName) : std::string(szName)
        {
            std::transform(this->begin(), this->end(), this->begin(), tolower);
        }

        bool IsFlexibleBone() const
        {
            return this->_Starts_with("fbr");
        }
    };

    class path : public std::filesystem::path
    {
    public:
        lower_string base;

        path() {}
        path(const char* szName) : std::filesystem::path(szName)
        {
            base = this->stem().string().c_str();
        }

        bool try_replace_extension(const char* ext)
        {
            std::filesystem::path tmp = *this;

            tmp.replace_extension(ext);
            CHECK_BOOL(std::filesystem::exists(tmp));
            this->replace_extension(ext);

            return true;
        }
    };
}