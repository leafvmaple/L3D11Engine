#include "stdafx.h"

#include "FilePath.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <io.h>

namespace L3D
{

	HRESULT PathSplit(const char* szFileName, char* szRetDir, size_t nDirSize, char* szRetBase, size_t nBaseSize)
	{
		HRESULT hr = E_FAIL;
		int nRetCode = 0;
		HRESULT hResult = E_FAIL;

		char szName[MAX_PATH];
		char szDriver[10];
		char szDir[MAX_PATH];

		nRetCode = _splitpath_s(szFileName, szDriver, sizeof(szDriver), szDir, sizeof(szDir), szName, sizeof(szName), NULL, 0);
		BOOL_ERROR_EXIT(nRetCode == 0);

		if (szRetDir)
		{
			nRetCode = sprintf_s(szRetDir, nDirSize, "%hs%hs", szDriver, szDir);
			BOOL_ERROR_EXIT(nRetCode > 0);
		}

		if (szRetBase)
		{
			nRetCode = sprintf_s(szRetBase, nBaseSize, "%hs", szName);
			BOOL_ERROR_EXIT(nRetCode > 0);
		}

		hResult = S_OK;
	Exit0:
		return hResult;
	}

	HRESULT GetExtName(char* szFileName, char* szExt, size_t nExtSize)
	{
		HRESULT hResult = E_FAIL;

		int nRetCode = _splitpath_s(szFileName, nullptr, 0, nullptr, 0, nullptr, 0, szExt, nExtSize);
		BOOL_ERROR_EXIT(nRetCode > 0);

		hResult = S_OK;
	Exit0:
		return hResult;
	}

	HRESULT FormatExtName(char* szFileName, const char* ExtName)
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

		BOOL_ERROR_EXIT(_access(szResult, 0) != -1);

		strcpy(szFileName, szResult);

		hResult = S_OK;
	Exit0:
		return hResult;
	}
}