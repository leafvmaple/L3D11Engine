#include "stdafx.h"

#include <wtypes.h>
#include <atlconv.h>

#include "L3DShader.h"
#include "IO/LFileReader.h"

#define FXO_PATH "data/material/Shader/Base/FXO/g_%s_%s_ASSEMBLY.fxo"

static const char* VERTEX_SHADER_ID_NAME[] =
{
    "VERTEX_SHADER_CI_MESH",
    "VERTEX_SHADER_CI_SKINMESH",
    "VERTEX_SHADER_CI_TERRAIN",
};

static const char* PIXEL_SHADER_ID_NAME[] =
{
    "PIXEL_SHADER_NULL",
    "PIXEL_SHADER_UV1_NO_LIGHT",
};

bool LoadShader(const char* szFileName, const char* szType, BYTE** ppByteCode, size_t* puSize)
{
    char szFilePath[MAX_PATH] {};

    if (sprintf_s(szFilePath, FXO_PATH, szType, szFileName) < 0)
        return false;

    if (FAILED(LFileReader::Read(szFilePath, ppByteCode, puSize)))
        return false;

    *puSize -= sizeof(GUID);

    return true;
}

bool CreateShaderTable(std::shared_ptr<L3D_SHADER_TABLE>* pShader, ID3D11Device* piDevice)
{
    std::shared_ptr<L3D_SHADER_TABLE> pShaderTable = std::make_shared<L3D_SHADER_TABLE>();
    if (!pShaderTable)
        return false;

    struct _SHADER_INFO
    {
        std::unique_ptr<BYTE[]> pByte;
        size_t nSize;
    };

    std::array<_SHADER_INFO, L3D_VERTEX_SHADER_COUNT> VertexShaderInfos;

    // Vertex Shader
    for (int i = 0; i < L3D_VERTEX_SHADER_COUNT; ++i)
    {
        auto& ShaderInfo = VertexShaderInfos[i];
        BYTE* pByte = nullptr;

        if (!LoadShader(VERTEX_SHADER_ID_NAME[i], "vs", &pByte, &ShaderInfo.nSize))
            return false;

        ShaderInfo.pByte.reset(pByte);
        if (FAILED(piDevice->CreateVertexShader(ShaderInfo.pByte.get(), ShaderInfo.nSize, nullptr, &pShaderTable->Vertex[i])))
            return false;
    }

    // Pixel Shader
    for (int i = L3D_PIXEL_SHADER_NULL + 1; i < L3D_PIXEL_SHADER_COUNT; ++i)
    {
        BYTE* pByte = nullptr;
        size_t uSize = 0;

        if (!LoadShader(PIXEL_SHADER_ID_NAME[i], "ps", &pByte, &uSize))
            return false;

        std::unique_ptr<BYTE[]> pByteGuard(pByte);
        if (FAILED(piDevice->CreatePixelShader(pByte, uSize, nullptr, &pShaderTable->Pixel[i])))
            return false;
    }

    // Input Layer
    for (int i = 0; i < L3D_INPUT_LAYOUT_COUNT; ++i)
    {
        const auto& LayoutInfo = INPUT_LAYOUT_LIST[i];
        const auto& ShaderInfo = VertexShaderInfos[LayoutInfo.eVertexShader];

        if (FAILED(piDevice->CreateInputLayout(LayoutInfo.DescArr,
            LayoutInfo.uDescCount,
            ShaderInfo.pByte.get(),
            ShaderInfo.nSize,
            &pShaderTable->Layout[i])))
            return false;
    }

    *pShader = std::move(pShaderTable);
    return true;
}
