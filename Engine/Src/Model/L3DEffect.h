#pragma once

#include <d3d11.h>
#include <vector>
#include <string>

#include "L3DMaterialConfig.h"

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectConstantBuffer;
struct ID3DX11EffectVariable;
struct ID3DX11EffectShaderResourceVariable;

struct EFFECT_SHADER
{
	std::string name;
	ID3DX11EffectShaderResourceVariable* pSRVaraible;
};

struct MESH_SHARED_CB;

class L3DEffect
{
public:
	HRESULT Create(ID3D11Device* piDevice, RUNTIME_MACRO eMacro);

	ID3DX11EffectTechnique* GetTechniqueByName(const char* szTechniqueName);

	void GetShader(std::vector<EFFECT_SHADER>& EffectShader);

	HRESULT SetVariableValue(MESH_SHARED_CB* pData);

	MESH_SHARED_CB* m_pData = nullptr;

private:
	struct MODEL_FIX_VARIBLES
	{
		ID3DX11EffectVariable* pModelParams;
	};

	ID3DX11Effect* m_piEffect = nullptr;
	ID3DX11EffectConstantBuffer* m_piModelSharedCB = nullptr;

	MODEL_FIX_VARIBLES m_Variables;

	std::vector<EFFECT_SHADER> m_vecShader;
};