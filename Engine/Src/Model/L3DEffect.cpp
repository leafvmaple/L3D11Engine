#include "stdafx.h"

#include <d3dcompiler.h>
#include <atlconv.h>
#include <cstdio>

#include "L3DEffect.h"

#define FX_PATH "Res/Shader/Base/%s.fx"

#ifdef _DEBUG
#pragma comment(lib, "Effects11d.lib")
#endif

static const char* EFFECT_SHADER_ID_NAME[] =
{
	"EFFECT_SHADER_BOX",
	"EFFECT_SHADER_CI_SKINMESH",
};

L3D_EFFECT_TABLE* CreateEffectTable(ID3D11Device* piDevice)
{
	HRESULT hr = E_FAIL;
	int nResult = false;
	int nRetCode = false;
	char szFilePath[MAX_PATH] = "\0";

	DWORD dwShaderFlags = 0;
	ID3DBlob* pCompiledShader = 0;
	ID3DBlob* pCompilationMsgs = 0;

	D3DX11_PASS_DESC passDesc;

	L3D_EFFECT_TABLE* pEffectTable = new L3D_EFFECT_TABLE;
	BOOL_ERROR_EXIT(pEffectTable);

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
	dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	for (int i = 0.; i < L3D_SHADER_EFFECT_COUNT; i++)
	{
		L3D_EFFECT_TABLE::_EFFECT_INFO& Table = pEffectTable->Table[i];
		const _SHADER_EFFECT_INFO& Info = SHADER_EFFECT_LIST[i];

		nRetCode = sprintf(szFilePath, FX_PATH, EFFECT_SHADER_ID_NAME[i]);
		BOOL_ERROR_EXIT(nRetCode);

		{
			USES_CONVERSION;
			hr = D3DCompileFromFile(A2CW((LPCSTR)szFilePath), 0, 0, 0, "fx_5_0", dwShaderFlags, 0, &pCompiledShader, &pCompilationMsgs);
			HRESULT_ERROR_EXIT(hr);

		}

		hr = D3DX11CreateEffectFromMemory(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), 0, piDevice, &Table.pEffect);
		HRESULT_ERROR_EXIT(hr);

		Table.pEffectTech = Table.pEffect->GetTechniqueByName(Info.szTechName);
		BOOL_ERROR_EXIT(Table.pEffectTech);

		Table.pWorldViewProj = Table.pEffect->GetVariableByName(Info.szMaxtrixName)->AsMatrix();
		BOOL_ERROR_EXIT(Table.pWorldViewProj);

		Table.pEffectTech->GetPassByIndex(0)->GetDesc(&passDesc);

		hr = piDevice->CreateInputLayout(Info.DescArr, Info.uDescCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &Table.pLayout);
		HRESULT_ERROR_EXIT(hr);
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		SAFE_DELETE(pEffectTable);
	}
	return pEffectTable;
}
