#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>

struct ID3DX11EffectShaderResourceVariable;

class L3DTexture;

struct TEXTURE_DATA
{
	std::string sRepresentName;
	std::string sRegisterName;

	L3DTexture* pTexture;
};

class L3DTexture
{
public:
	HRESULT Create(ID3D11Device* piDevice, const char* szFileName);
	HRESULT Apply(ID3DX11EffectShaderResourceVariable* pEffectSRVariable);

	ID3D11Texture2D* m_piTexture;
	ID3D11ShaderResourceView* m_piTextureView;
};