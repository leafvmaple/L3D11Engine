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

    private:
        void _to_lower() {
            std::transform(this->begin(), this->end(), this->begin(), tolower);
        }
    };

    class path : public std::filesystem::path
    {
    public:
        path() {}
        path(const std::filesystem::path& r) : std::filesystem::path(r) {
            _flush();
        };
        path(const char* szName) : std::filesystem::path(szName) {
            _flush();
        }

        const lower_string& lower_extension() {
            return _extension;
        }

        bool try_replace_extension(const char* ext) {
            std::filesystem::path tmp = *this;

            tmp.replace_extension(ext);
            CHECK_BOOL(std::filesystem::exists(tmp));

            replace_extension(ext);
            _flush();

            return true;
        }

    private:
        lower_string _base;
        lower_string _extension;

        void _flush() {
            _base = stem().string();
            _extension = extension().string();
        }
    };
}