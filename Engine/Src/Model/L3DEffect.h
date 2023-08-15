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

struct EFFECT_SHADER
{
    std::string name;
    ID3DX11EffectShaderResourceVariable* pSRVaraible;
};

class L3DEffect
{
public:
    HRESULT Create(ID3D11Device* piDevice, const char* szMaterial, RUNTIME_MACRO eMacro);

    ID3DX11EffectTechnique* GetTechniqueByName(const char* szTechniqueName);
    ID3DX11EffectConstantBuffer* GetConstantBufferByRegister(int nIndex);
    ID3DX11EffectConstantBuffer* GetConstantBufferByName(const char* szCBName);

    void GetShader(std::vector<EFFECT_SHADER>& EffectShader);

private:
    ID3DX11Effect* m_piEffect = nullptr;

    std::vector<EFFECT_SHADER> m_vecShader;
    std::unordered_map<int, int> REG_TO_IDX;
};