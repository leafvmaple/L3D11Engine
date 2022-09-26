#include "stdafx.h"

#include <d3dcompiler.h>
#include <atlconv.h>

#include <string>
#include "FX11/inc/d3dx11effect.h"

#include "L3DMaterial.h"
#include "L3DInterface.h"
#include "L3DMaterialConfig.h"

#define MATERIAL_SHADER_ROOT "Res/Shader/Material/"

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

HRESULT L3DMaterial::Create(ID3D11Device* piDevice, RUNTIME_MACRO eMacro)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    DWORD dwShaderFlags = 0;
    ID3DBlob* pCompiledShader = nullptr;
    ID3DBlob* pCompilationMsgs = nullptr;
    ID3DX11Effect* pEffect = nullptr;

#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3D10_SHADER_DEBUG;
    dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    {
        USES_CONVERSION;
        hr = D3DCompileFromFile(A2CW((LPCSTR)gs_ShaderTemplate[eMacro].value), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 0, "fx_5_0", dwShaderFlags, 0, &pCompiledShader, &pCompilationMsgs);
        {
            std::string str;
            if (pCompilationMsgs)
                str = static_cast<char*>(pCompilationMsgs->GetBufferPointer());
        }
        HRESULT_ERROR_EXIT(hr);

        hr = D3DX11CreateEffectFromMemory(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), 0, piDevice, &pEffect);
        HRESULT_ERROR_EXIT(hr);

        m_piModelSharedCB = pEffect->GetConstantBufferByName("ModelSharedParam");
        BOOL_ERROR_EXIT(m_piModelSharedCB);

        m_Variables.pModelParams = m_piModelSharedCB->GetMemberByName("g_ModelParams");
        BOOL_ERROR_EXIT(m_Variables.pModelParams);

		m_piEffectTech = pEffect->GetTechniqueByName("Color");
		BOOL_ERROR_EXIT(m_piEffectTech);
    }

    m_pData = new MESH_SHARED_CB;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMaterial::Apply(ID3D11DeviceContext* pDeviceContext)
{
	D3DX11_TECHNIQUE_DESC techDesc;

	m_piEffectTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
		m_piEffectTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
    }

	return S_OK;
}

HRESULT L3DMaterial::SetVariableValue(MESH_SHARED_CB* pData)
{
    m_Variables.pModelParams->SetRawValue(pData, 0, sizeof(MESH_SHARED_CB));

    return S_OK;
}