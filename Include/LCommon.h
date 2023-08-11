#pragma once
#include <winnt.h>
#include <winerror.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <io.h>

#include "LAssert.h"

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

namespace L3D
{
    inline size_t GetPathDir(const wchar_t* wszFilePath, WCHAR* wszDir)
    {
        size_t uLength = wcslen(wszFilePath);
        for (; uLength > 0; uLength--)
        {
            if (wszFilePath[uLength] == '/')
            {
                wcsncpy_s(wszDir, FILENAME_MAX, wszFilePath, uLength + 1);
                return uLength + 1;
            }
        }
        return 0;
    }

    inline size_t GetFullPath(const wchar_t* wszFileName, WCHAR* wszPath)
    {
        size_t uLength = wcslen(wszFileName);
        for (size_t u = 0; u < uLength; u++)
        {
            if (wszFileName[u] == '.' || wszFileName[u] == '\\' || wszFileName[u] == '/')
                continue;
            wcscat_s(wszPath, FILENAME_MAX, &wszFileName[u]);
            return wcslen(wszPath);
        }
        return 0;
    }
}