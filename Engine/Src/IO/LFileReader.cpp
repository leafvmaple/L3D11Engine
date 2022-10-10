#include "stdafx.h"

#include <cstdio>
#include <io.h>

#include "LFileReader.h"

BOOL LFileReader::IsExist(LPCWSTR cszFileName)
{
	return _waccess(cszFileName, 0) != -1;
}

HRESULT LFileReader::Read(LPCWSTR cszFileName, BYTE** ppBuffer, size_t* puLen)
{
	HRESULT hResult = E_FAIL;
	FILE* pFile = NULL;
	BYTE* pBuffer = NULL;
	size_t uFileLen;

	do
	{
		BOOL_ERROR_BREAK(ppBuffer);

		_wfopen_s(&pFile, cszFileName, TEXT("rb"));
		BOOL_ERROR_BREAK(pFile);

		fseek(pFile, 0, SEEK_END);
		uFileLen = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		pBuffer = new BYTE[uFileLen];
		BOOL_ERROR_BREAK(pBuffer);

		*puLen = fread(pBuffer, 1, uFileLen, pFile);
		BOOL_ERROR_BREAK(*puLen);

		*ppBuffer = pBuffer;

		hResult = S_OK;
	} while (0);

	if (pFile)
		fclose(pFile);

	return hResult;
}

HRESULT LFileReader::ReadJson(const char* szFileName, rapidjson::Document& JsonDocument)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	BYTE* pData = nullptr;
	size_t uSize = 0;

	LFileReader::Read(szFileName, &pData, &uSize);
	BOOL_ERROR_EXIT(pData);

	JsonDocument.Parse((char*)pData, uSize);
	BOOL_ERROR_EXIT(!JsonDocument.HasParseError());

	hResult = S_OK;
Exit0:
	SAFE_DELETE(pData);
	return hResult;
}

