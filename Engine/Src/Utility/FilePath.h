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
    HRESULT ReplaceExtName(char* szFileName, const char* ExtName);

    void PathFormat(const char* szFileName, std::filesystem::path& path);
}