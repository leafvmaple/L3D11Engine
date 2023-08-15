#include "stdafx.h"
#include "L3DEffect.h"

#include <d3dcompiler.h>
#include <atlconv.h>
#include <string>

#include "L3DMaterialConfig.h"

#include "FX11/inc/d3dx11effect.h"

#define MATERIAL_SHADER_ROOT "data/material/Shader/"

struct _MacroKeyValue
{
    RUNTIME_MACRO key;
    const char* value;
};

static _MacroKeyValue gs_ShaderTemplate[] = {
    { RUNTIME_MACRO_MESH,        MATERIAL_SHADER_ROOT"MeshShader.fx5" },
    // { RUNTIME_MACRO_SKIN_MESH,   MATERIAL_SHADER_ROOT"SkinMeshShader.fx5"},
    { RUNTIME_MACRO_SKIN_MESH,   MATERIAL_SHADER_ROOT"SimpleSkinMeshShader.fx5"},
};

L3DInclude::L3DInclude(const char* szFileName)
{
    m_sName = szFileName;
}


L3DInclude::~L3DInclude()
{

}

HRESULT STDMETHODCALLTYPE L3DInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
    HRESULT hReuslt = E_FAIL;
    FILE* f = nullptr;
    int nFileLen = 0;
    char* pBuffer = nullptr;
    std::string fileName = !_stricmp(pFileName, "UserShader.fx5") ? m_sName : std::string(MATERIAL_SHADER_ROOT) + pFileName;

    f = fopen(fileName.c_str(), "rb");
    BOOL_ERROR_EXIT(f);

    fseek(f, 0, SEEK_END);
    nFileLen = ftell(f);
    fseek(f, 0, SEEK_SET);

    pBuffer = new char[nFileLen + 1];
    fread(pBuffer, sizeof(char), nFileLen + 1, f);

    *pBytes = nFileLen;
    *ppData = pBuffer;

    m_piBufferList.push_back(pBuffer);

    hReuslt = S_OK;
Exit0:
    return hReuslt;
}

HRESULT STDMETHODCALLTYPE L3DInclude::Close(LPCVOID pData)
{
    SAFE_DELETE(pData);

    return S_OK;
}

HRESULT L3DEffect::Create(ID3D11Device* piDevice, const char* szMaterial, RUNTIME_MACRO eMacro)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    DWORD dwShaderFlags = 0;
    ID3DBlob* pCompiledShader = nullptr;
    ID3DBlob* pCompilationMsgs = nullptr;
    ID3DX11Effect* pEffect = nullptr;
    ID3DX11EffectVariable* pVariable = nullptr;
    ID3DX11EffectShaderResourceVariable* pSRVariable = nullptr;
    ID3DX11EffectType* pType = nullptr;

    D3DX11_EFFECT_DESC desc;
    D3DX11_EFFECT_VARIABLE_DESC variableDesc;
    D3DX11_EFFECT_TYPE_DESC typeDesc;

    L3DInclude include(szMaterial);
    char error[MAX_PATH];

#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3D10_SHADER_DEBUG;
    dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    {
        USES_CONVERSION;
        hr = D3DCompileFromFile(A2CW((LPCSTR)gs_ShaderTemplate[eMacro].value), 0, &include, 0, "fx_5_0", dwShaderFlags, 0, &pCompiledShader, &pCompilationMsgs);
        if (FAILED(hr))
        {
            
            strcpy(error, (const char*)pCompilationMsgs->GetBufferPointer());
        }
        HRESULT_ERROR_EXIT(hr);
    }

    hr = D3DX11CreateEffectFromMemory(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), 0, piDevice, &m_piEffect);
    HRESULT_ERROR_EXIT(hr);

    hr = m_piEffect->GetDesc(&desc);
    HRESULT_ERROR_EXIT(hr);

    for (int i = 0; i < desc.GlobalVariables; i++)
    {
        pVariable = m_piEffect->GetVariableByIndex(i);
        BOOL_ERROR_EXIT(pVariable);

        pVariable->GetDesc(&variableDesc);
        pType = pVariable->GetType();
        BOOL_ERROR_EXIT(pType);

        pType->GetDesc(&typeDesc);
        BOOL_ERROR_EXIT(pType);

        if (typeDesc.Type == D3D_SVT_TEXTURE2D)
        {
            pSRVariable = pVariable->AsShaderResource();
            m_vecShader.push_back({ variableDesc.Name, pSRVariable });
        }
    }

    for (int i = 0; i < desc.ConstantBuffers; i++)
    {
        auto pVariable = m_piEffect->GetConstantBufferByIndex(i);
        BOOL_ERROR_EXIT(pVariable);

        pVariable->GetDesc(&variableDesc);
        pType = pVariable->GetType();
        BOOL_ERROR_EXIT(pType);

        pType->GetDesc(&typeDesc);
        BOOL_ERROR_EXIT(pType);

        REG_TO_IDX[variableDesc.ExplicitBindPoint] = i;
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

ID3DX11EffectTechnique* L3DEffect::GetTechniqueByName(const char* szTechniqueName)
{
    return m_piEffect->GetTechniqueByName(szTechniqueName);
}

ID3DX11EffectConstantBuffer* L3DEffect::GetConstantBufferByRegister(int nIndex)
{
    return m_piEffect->GetConstantBufferByIndex(REG_TO_IDX[nIndex]);
}

ID3DX11EffectConstantBuffer* L3DEffect::GetConstantBufferByName(const char* szCBName)
{
    return m_piEffect->GetConstantBufferByName(szCBName);
}

void L3DEffect::GetShader(std::vector<EFFECT_SHADER>& EffectShader)
{
    EffectShader = m_vecShader;
}
