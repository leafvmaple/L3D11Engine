#pragma once

#include <d3d11.h>
#include <vector>
#include <string>

#include "L3DMaterialConfig.h"

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectConstantBuffer;
struct ID3DX11EffectShaderResourceVariable;

struct EFFECT_SHADER
{
	std::string name;
	ID3DX11EffectShaderResourceVariable* pSRVaraible;
};

class L3DEffect
{
public:
	HRESULT Create(ID3D11Device* piDevice, RUNTIME_MACRO eMacro);

	ID3DX11EffectTechnique* GetTechniqueByName(const char* szTechniqueName);
	ID3DX11EffectConstantBuffer* GetConstantBufferByName(const char* szCBName);

	void GetShader(std::vector<EFFECT_SHADER>& EffectShader);

private:
	ID3DX11Effect* m_piEffect = nullptr;

	std::vector<EFFECT_SHADER> m_vecShader;
};