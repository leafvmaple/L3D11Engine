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

    std::filesystem::path filename = szFileName;

    L3D::TryReplaceExtension(filename, ".dds");
    std::filesystem::path extension = filename.extension();

    if (extension == ".dds")
    {
        hr = LoadFromDDSFile(filename.wstring().c_str(), DDS_FLAGS_NONE, nullptr, LoadedImage);
        HRESULT_ERROR_LOG_EXIT(hr, "Load texture [{}] failed", filename.string());
    }
    else if (extension == ".tga")
    {
        hr = LoadFromTGAFile(filename.wstring().c_str(), nullptr, LoadedImage);
        HRESULT_ERROR_LOG_EXIT(hr, "Load texture [{}] failed", filename.string());
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

template<typename T>
HRESULT L3DTexture::Create(ID3D11Device* piDevice, T* pData, UINT nWidth, UINT nHeight)
{
    D3D11_TEXTURE2D_DESC texDesc;
    D3D11_SUBRESOURCE_DATA texData;

    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.Format = DXGI_FORMAT_R32_FLOAT;
    texDesc.Width = nWidth;
    texDesc.Height = nHeight;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    texData.pSysMem = pData;
    texData.SysMemPitch = nWidth * sizeof(float);
    texData.SysMemSlicePitch = 0;

    piDevice->CreateTexture2D(&texDesc, &texData, &m_piTexture);
    piDevice->CreateShaderResourceView(m_piTexture, nullptr, &m_piTextureView);

    return S_OK;
}

HRESULT L3DTexture::Apply(ID3DX11EffectShaderResourceVariable* pEffectSRVariable)
{
    return pEffectSRVariable->SetResource(m_piTextureView);
}
