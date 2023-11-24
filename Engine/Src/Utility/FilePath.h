#pragma once

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#include <winerror.h>
#include <filesystem>

namespace L3D
{
    HRESULT PathSplit(const char* szFileName, char* szRetDir, size_t nDirSize, char* szRetBase, size_t nBaseSize);

    HRESULT GetExtName(const char* szFileName, char* szExt, size_t nExtSize);
    void GetExtension(std::filesystem::path& path, wchar_t* szExt);
    HRESULT ReplaceExtName(char* szFileName, const char* ExtName);

    void PathFormat(const char* szFileName, std::filesystem::path& path);

    void TryReplaceExtension(std::filesystem::path& path, const std::filesystem::path& ext);
}