#include "stdafx.h"
#include <atlconv.h>

#include "L3DTexture.h"

#include "Utility/FilePath.h"

#include "FX11/inc/d3dx11effect.h"
#include "DirectXTex/DirectXTex/DirectXTex.h"
#include "rapidjson/include/rapidjson/rapidjson.h"

using namespace DirectX;

HRESULT L3DTexture::Create(ID3D11Device* piDevice, const char* szFileName)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	ScratchImage LoadedImage;
	ID3D11Resource* piResource = nullptr;
	ID3D11ShaderResourceView* piSRV = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	char szName[MAX_PATH];
	char szBaseName[MAX_PATH];

	strcpy(szName, szFileName);

	L3D::ReplaceExtName(szName, ".dds");
	L3D::GetExtName(szName, szBaseName, MAX_PATH);

	{
		USES_CONVERSION;

		const wchar_t* wzName = A2CW(szName);
		BOOL_ERROR_EXIT(wzName);

		if (!strcmp(szBaseName, ".dds"))
		{
			hr = LoadFromDDSFile(wzName, DDS_FLAGS_NONE, nullptr, LoadedImage);
			HRESULT_ERROR_EXIT(hr);
		}
		else if (!strcmp(szBaseName, ".tga"))
		{
			hr = LoadFromTGAFile(wzName, nullptr, LoadedImage);
			HRESULT_ERROR_EXIT(hr);
		}
	}

	hr = CreateTexture(piDevice, LoadedImage.GetImages(), LoadedImage.GetImageCount(), LoadedImage.GetMetadata(), &piResource);
	HRESULT_ERROR_EXIT(hr);

	hr = piResource->QueryInterface(IID_ID3D11Texture2D, (void**)&m_piTexture);
	HRESULT_ERROR_EXIT(hr);

	/*if (IsCompressed(LoadedImage.GetMetadata().format))
	{
		ScratchImage DecompressImage;

		hr = Decompress(*LoadedImage.GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM, DecompressImage);
		HRESULT_ERROR_EXIT(hr);

		LoadedImage = std::move(DecompressImage);
	}*/

	srvDesc.Format        = LoadedImage.GetMetadata().format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	srvDesc.Texture2D.MipLevels       = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = piDevice->CreateShaderResourceView(piResource, &srvDesc, &m_piTextureView);
	HRESULT_ERROR_EXIT(hr);

	hResult = S_OK;
Exit0:
	return hResult;

}

HRESULT L3DTexture::Apply(ID3DX11EffectShaderResourceVariable* pEffectSRVariable)
{
	return pEffectSRVariable->SetResource(m_piTextureView);
}
