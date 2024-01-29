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
    char szFilePath[MAX_PATH] = { 0 };

    CHECK_BOOL(sprintf_s(szFilePath, FXO_PATH, szType, szFileName));
    CHECK_HRESULT(LFileReader::Read(szFilePath, ppByteCode, puSize));

    *puSize -= sizeof(GUID);
}

bool CreateShaderTable(std::shared_ptr<L3D_SHADER_TABLE>* pShader, ID3D11Device* piDevice)
{
    BYTE*   pByte   = nullptr;
    size_t  uSize   = 0;

    std::shared_ptr<L3D_SHADER_TABLE> pShaderTable = nullptr;

    struct _SHADER_INFO
    {
        BYTE*  pByte;
        size_t nSize;
    } VertexShaderInfos[L3D_VERTEX_SHADER_COUNT] = { 0 };

    pShaderTable = std::make_shared<L3D_SHADER_TABLE>();
    CHECK_BOOL(pShaderTable);

    // Vertex Shader
    for (int i = 0; i < L3D_VERTEX_SHADER_COUNT; i++)
    {
        _SHADER_INFO& ShaderInfo = VertexShaderInfos[i];

        CHECK_BOOL(LoadShader(VERTEX_SHADER_ID_NAME[i], "vs", &ShaderInfo.pByte, &ShaderInfo.nSize));
        CHECK_HRESULT(piDevice->CreateVertexShader(ShaderInfo.pByte, ShaderInfo.nSize, nullptr, &pShaderTable->Vertex[i]));
    }

    // Pixel Shader
    for (int i = L3D_PIXEL_SHADER_NULL + 1; i < L3D_PIXEL_SHADER_COUNT; i++)
    {
        CHECK_BOOL(LoadShader(PIXEL_SHADER_ID_NAME[i], "ps", &pByte, &uSize));
        CHECK_HRESULT(piDevice->CreatePixelShader(pByte, uSize, nullptr, &pShaderTable->Pixel[i]));
    }

    // Input Layer
    for (int i = 0; i < L3D_INPUT_LAYOUT_COUNT; i++)
    {
        const _LAYOUT_INFO& LayoutInfo = INPUT_LAYOUT_LIST[i];

        CHECK_HRESULT(piDevice->CreateInputLayout(LayoutInfo.DescArr,
            LayoutInfo.uDescCount,
            VertexShaderInfos[LayoutInfo.eVertexShader].pByte,
            VertexShaderInfos[LayoutInfo.eVertexShader].nSize,
            &pShaderTable->Layout[i]
        ));
    }
    *pShader = pShaderTable;
    return true;
}
