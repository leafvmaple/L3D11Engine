#pragma once
#include "LFileStruct.h"
#include <wtypes.h>
#include <atlconv.h>
#include <cstdio>

#include "rapidjson/include/rapidjson/document.h"
#include "simpleini/SimpleIni.h"


class LFileReader
{
public:
	static BOOL IsExist(LPCWSTR cszFileName);
	static HRESULT Read(LPCWSTR cszFileName, BYTE** ppBuffer, size_t* puLen);
	static HRESULT Read(const char* szFileName, BYTE** ppBuffer, size_t* puLen)
	{
		USES_CONVERSION;
		return Read(A2CW((LPCSTR)szFileName), ppBuffer, puLen);
	}

	static HRESULT ReadJson(const char* szFileName, rapidjson::Document& JsonDocument);

	static HRESULT ReadIni(const char* szFileName, CSimpleIniA& Ini);

	template<typename Model>
	static BYTE* Convert(BYTE* pBuffer, Model*& pModel, size_t nCount = 1)
	{
		size_t nLen = sizeof(Model) * nCount;
		pModel = (Model*)pBuffer;
		return pBuffer + nLen;
	}

	template<typename Model>
	static BYTE* Convert(BYTE* pBuffer, Model& pModel)
	{
		pModel = *((Model*)pBuffer);
		return pBuffer + sizeof(Model);
	}

	template<typename Model, int nSize>
	static BYTE* Copy(BYTE* pBuffer, Model pModel[nSize])
	{
		size_t nLen = sizeof(Model) * nSize;
		memcpy_s(&pModel, nLen, pBuffer, nLen);
		return pBuffer + nLen;
	}

	static BYTE* Copy(BYTE* pBuffer, TCHAR pModel[], size_t nLen = 0)
	{
		if (!nLen)
			nLen = strlen((LPCSTR)pBuffer);
		{
			USES_CONVERSION;
			wcscpy_s(pModel, nLen, A2CW((LPCSTR)pBuffer));
		}
		return pBuffer + nLen;
	}

	template<typename Model>
	static BYTE* Copy(BYTE* pBuffer, Model* pModel, size_t nCount = 1)
	{
		size_t nLen = sizeof(Model) * nCount;
		memcpy_s(pModel, nLen, pBuffer, nLen);
		return pBuffer + nLen;
	}
};