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
        lower_string(const std::string& str) : std::string(str) {
            _to_lower();
        }
        lower_string(const char* sz) : std::string(sz) {
            _to_lower();
        }

        bool flexible_bone() const {
            return this->starts_with("fbr");
        }

    private:
        void _to_lower() {
            std::transform(this->begin(), this->end(), this->begin(), tolower);
        }
    };

    class path : public std::filesystem::path
    {
    public:
        lower_string base;
        lower_string ext;

        path() {}
        path(const std::filesystem::path& r) : std::filesystem::path(r) {
            _flush();
        };
        path(const char* szName) : std::filesystem::path(szName) {
            _flush();
        }

        bool try_replace_extension(const char* ext) {
            std::filesystem::path tmp = *this;

            tmp.replace_extension(ext);
            CHECK_BOOL(std::filesystem::exists(tmp));
            this->replace_extension(ext);

            _flush();

            return true;
        }

    private:
        void _flush() {
            base = this->stem().string();
            ext = this->extension().string();
        }
    };
}