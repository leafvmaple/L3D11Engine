#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>

#include "Utility/L3DString.h"

struct ID3DX11EffectShaderResourceVariable;

class L3DTexture;

struct TEXTURE_DATA
{
	std::string sRepresentName;
	std::string sRegisterName;

	L3DTexture* pTexture = nullptr;
};

class L3DTexture
{
public:
	L3D::path m_Path;

	HRESULT Create(ID3D11Device* piDevice, const char* szFileName);
	template<typename T>
    HRESULT Create(ID3D11Device* piDevice, T* pData, UINT nWidth, UINT nHeight);
	HRESULT Apply(ID3DX11EffectShaderResourceVariable* pEffectSRVariable);

	ID3D11Texture2D* m_piTexture = nullptr;
	ID3D11ShaderResourceView* m_piTextureView = nullptr;
};

template HRESULT L3DTexture::Create<float>(ID3D11Device* piDevice, float* pData, UINT nWidth, UINT nHeight);