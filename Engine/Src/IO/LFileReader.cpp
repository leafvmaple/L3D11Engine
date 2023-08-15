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

HRESULT LFileReader::ReadJson(const wchar_t* szFileName, rapidjson::Document& JsonDocument)
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

HRESULT LFileReader::ReadIni(const char* szFileName, CSimpleIniA& Ini)
{
	HRESULT hResult = E_FAIL;

	SI_Error rc = Ini.LoadFile(szFileName);
	BOOL_ERROR_EXIT(rc >= 0);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT LBinaryReader::Init(const char* szFileName)
{
	USES_CONVERSION;
	return Init(A2CW((LPCSTR)szFileName));
}

HRESULT LBinaryReader::Init(LPCWSTR cszFileName)
{
	HRESULT hResult = E_FAIL;
	FILE* pFile = nullptr;
	size_t nFileLen = 0;

	_wfopen_s(&pFile, cszFileName, TEXT("rb"));
	BOOL_ERROR_EXIT(pFile);

	fseek(pFile, 0, SEEK_END);
	nFileLen = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	m_pBuffer = new (std::nothrow) BYTE[nFileLen];
	BOOL_ERROR_EXIT(m_pBuffer);

	m_nLength = fread(m_pBuffer, 1, nFileLen, pFile);
	BOOL_ERROR_EXIT(m_nLength);
	BOOL_ERROR_EXIT(m_nLength == nFileLen);

	m_pCursor = m_pBuffer;

	hResult = S_OK;
Exit0:
	if (pFile)
		fclose(pFile);
	if (FAILED(hResult))
	{
		SAFE_DELETE(m_pBuffer);
		m_nLength = 0;
	}

	return hResult;
}

