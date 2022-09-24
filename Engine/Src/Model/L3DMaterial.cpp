#include <d3dcompiler.h>
#include <atlconv.h>
#include <string>

#include "LAssert.h"
#include "FX11/inc/d3dx11effect.h"
#include "L3DInterface.h"

#include "L3DMaterial.h"

#define MATERIAL_SHADER_ROOT "Res/Shader/Material/"

struct _MacroKeyValue
{
	RUNTIME_MACRO key;
	const char* value;
};

static _MacroKeyValue gs_ShaderTemplate[] = {
	{ RUNTIME_MACRO_MESH,        MATERIAL_SHADER_ROOT"MeshShader.fx5" },
	{ RUNTIME_MACRO_SKIN_MESH,   MATERIAL_SHADER_ROOT"SkinMeshShader.fx5"},
};

L3DMaterial* LoadMaterial(ID3D11Device* piDevice, RUNTIME_MACRO eMacro, const char* szFile)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	DWORD dwShaderFlags = 0;
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pCompilationMsgs = nullptr;
	ID3DX11Effect* pEffect = nullptr;
	ID3DX11EffectConstantBuffer* piECB = nullptr;

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

		piECB = pEffect->GetConstantBufferByName("ModelSharedParam");
		BOOL_ERROR_EXIT(piECB);
	}

	hResult = S_OK;
Exit0:
	return nullptr;
}
