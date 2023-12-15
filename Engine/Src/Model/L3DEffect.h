#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "L3DMaterialConfig.h"

class L3DInclude : public ID3DInclude
{
public:
    L3DInclude(const char* szFileName);
    virtual ~L3DInclude();

    virtual HRESULT STDMETHODCALLTYPE Open(
        D3D_INCLUDE_TYPE IncludeType,
        LPCSTR pFileName,
        LPCVOID pParentData,
        LPCVOID* ppData,
        UINT* pBytes
    ) override;

    virtual HRESULT STDMETHODCALLTYPE Close(LPCVOID pData) override;

protected:
    std::string m_sName;
};

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectConstantBuffer;
struct ID3DX11EffectShaderResourceVariable;

struct L3DTexture;

struct EFFECT_SHADER
{
    std::string name;
    ID3DX11EffectShaderResourceVariable* pSRVariable;
};

class L3DEffect
{
public:
    HRESULT Create(ID3D11Device* piDevice, const char* szMaterial, RUNTIME_MACRO eMacro);

    ID3DX11EffectTechnique* GetTechniqueByName(const char* szTechniqueName);
    ID3DX11EffectConstantBuffer* GetConstantBufferByRegister(int nIndex);
    ID3DX11EffectConstantBuffer* GetConstantBufferByName(const char* szCBName);

    void GetTextures(std::unordered_map<std::string, ID3DX11EffectShaderResourceVariable*>& EffectShader);
    void SetTexture(const char* szName, L3DTexture* pTexture);

private:
    ID3DX11Effect* m_piEffect = nullptr;

    std::unordered_map <std::string, ID3DX11EffectShaderResourceVariable*> m_Textures;
    std::unordered_map<int, int> REG_TO_IDX;
};