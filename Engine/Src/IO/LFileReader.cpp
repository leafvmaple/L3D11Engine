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
    if (!ppBuffer) return E_FAIL;

    FILE* pFileRaw = nullptr;
    _wfopen_s(&pFileRaw, cszFileName, L"rb");
    std::unique_ptr<FILE, decltype(&fclose)> pFile(pFileRaw, fclose);
    if (!pFile) return E_FAIL;

    fseek(pFile.get(), 0, SEEK_END);
    size_t uFileLen = ftell(pFile.get());
    fseek(pFile.get(), 0, SEEK_SET);

    auto pBuffer = std::make_unique<BYTE[]>(uFileLen);
    if (!pBuffer) return E_FAIL;

    *puLen = fread(pBuffer.get(), 1, uFileLen, pFile.get());
    if (*puLen != uFileLen) return E_FAIL;

    *ppBuffer = pBuffer.release();
    return S_OK;
}

HRESULT LFileReader::ReadJson(const wchar_t* szFileName, rapidjson::Document& JsonDocument)
{
    BYTE* pData = nullptr;
    size_t uSize = 0;

    if (FAILED(LFileReader::Read(szFileName, &pData, &uSize)) || !pData) return E_FAIL;

    std::unique_ptr<BYTE[]> dataGuard(pData);
    JsonDocument.Parse(reinterpret_cast<char*>(pData), uSize);
    if (JsonDocument.HasParseError()) return E_FAIL;

    return S_OK;
}

HRESULT LFileReader::ReadJson(const char* szFileName, rapidjson::Document& JsonDocument)
{
    BYTE* pData = nullptr;
    size_t uSize = 0;

    if (FAILED(LFileReader::Read(szFileName, &pData, &uSize)) || !pData) return E_FAIL;

    std::unique_ptr<BYTE[]> dataGuard(pData);
    JsonDocument.Parse(reinterpret_cast<char*>(pData), uSize);
    if (JsonDocument.HasParseError()) return E_FAIL;

    return S_OK;
}

HRESULT LFileReader::ReadIni(const wchar_t* szFileName, CSimpleIniA& Ini)
{
    return Ini.LoadFile(szFileName) >= 0 ? S_OK : E_FAIL;
}

HRESULT LBinaryReader::Init(const char* szFileName)
{
    USES_CONVERSION;
    return Init(A2CW((LPCSTR)szFileName));
}

HRESULT LBinaryReader::Init(LPCWSTR cszFileName)
{
    FILE* pFileRaw = nullptr;
    _wfopen_s(&pFileRaw, cszFileName, L"rb");
    std::unique_ptr<FILE, decltype(&fclose)> pFile(pFileRaw, fclose);
    if (!pFile) return E_FAIL;

    fseek(pFile.get(), 0, SEEK_END);
    size_t nFileLen = ftell(pFile.get());
    fseek(pFile.get(), 0, SEEK_SET);

    m_pBuffer = std::make_unique<BYTE[]>(nFileLen).release();
    if (!m_pBuffer) return E_FAIL;

    m_nLength = fread(m_pBuffer, 1, nFileLen, pFile.get());
    if (m_nLength != nFileLen) return E_FAIL;

    m_pCursor = m_pBuffer;
    return S_OK;
}

