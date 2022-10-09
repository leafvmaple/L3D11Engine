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
    inline size_t GetPathDir(LPCWSTR wszFilePath, WCHAR* wszDir)
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

    inline size_t GetFullPath(LPCWSTR wszFileName, WCHAR* wszPath)
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

    inline HRESULT FormatExtName(char* szFileName, const char* ExtName)
    {
        HRESULT hr = E_FAIL;
        int nRetCode = 0;
        HRESULT hResult = E_FAIL;

		char szResult[MAX_PATH];
        char szName[MAX_PATH];
		char szDriver[10];
		char szDir[MAX_PATH];

		nRetCode = _splitpath_s(szFileName, szDriver, sizeof(szDriver), szDir, sizeof(szDir), szName, sizeof(szName), NULL, 0);
        BOOL_ERROR_EXIT(nRetCode == 0);

		nRetCode = sprintf_s(szResult, "%hs%hs%hs%hs", szDriver, szDir, szName, ExtName);
		BOOL_ERROR_EXIT(nRetCode > 0);

        if (_access(szResult, 0) != -1)
            strcpy(szFileName, szResult);

        hResult = S_OK;
    Exit0:
        return hResult;
    }
}