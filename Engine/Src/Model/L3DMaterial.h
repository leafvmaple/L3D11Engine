#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <d3d11.h>

#include "L3DFormat.h"

#include "L3DTexture.h"
#include "L3DMaterialConfig.h"

#include "rapidjson/include/rapidjson/document.h"

class L3DEffect;

struct L3D_STATE_TABLE;
struct MATERIAL_SOURCE;

struct ID3DX11EffectPass;
struct ID3DX11EffectConstantBuffer;
struct ID3DX11EffectShaderResourceVariable;

enum class MATERIAL_INDIV_CB
{
    MODELSHARED,
    SUBSET,
};

__declspec(align(16)) struct SKIN_SUBSET_CONST
{
    XMFLOAT4A   ModelColor;
    BOOL        EnableAlphaTest;
    float       AlphaReference;
    float       AlphaReference2;
};

enum BlendMode
{
    BLEND_OPAQUE,
    BLEND_MASKED,
    BLEND_SOFTMASKED = 5,
};

class L3DMaterialData
{
public:
    HRESULT Create(const char* szFileName);
    HRESULT GetTextureVariables(ID3D11Device* piDevice, std::vector<TEXTURE_DATA>& Variables);

    char m_szShaderName[256] = { 0 };
private:
    char m_szName[256] = { 0 };

    struct MaterialTextureTable
    {
        std::string hsRepresentName;
        std::string hsRegisterName;
        std::string tValue;
    };

    std::vector<MaterialTextureTable> m_vecTexture;
};

// KG3D_MATERIAL_INSTANCE_DATA
class L3DMaterial
{
public:
    ~L3DMaterial() {}

    HRESULT Create(ID3D11Device* piDevice, const MATERIAL_SOURCE& source, RUNTIME_MACRO eMacro);
    HRESULT ApplyRenderState(ID3D11DeviceContext* pDeviceContext, const std::shared_ptr<L3D_STATE_TABLE>& pStateTable);
    HRESULT Apply(ID3D11DeviceContext* pDeviceContext, RENDER_PASS ePass);

    void SetTexture(const char* szName, L3DTexture* pTexture);

    HRESULT CreateIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer** pEffectCB);
    HRESULT SetIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer* pSharedCB);

    void GetRenderStateValue(SKIN_SUBSET_CONST & subsetConst);

    BlendMode m_eBlendMode = BLEND_OPAQUE;
private:
    HRESULT _PlaceTextureValue(ID3D11Device* piDevice, std::string sName, std::string sTextureName);

    HRESULT _UpdateTechniques(RENDER_PASS ePass, ID3DX11EffectPass** ppEffectPass);
    HRESULT _UpdateTextures();
    void _UpdateCommonCB();

    L3DEffect*       m_pEffect         = nullptr;
    ID3D11Buffer*    m_piCommonCB      = nullptr;

    BOOL m_AlphaTestSwitch = false;
    unsigned int m_dwAlphaRef = 0;
    unsigned int m_dwAlphaRef2 = 0;
    unsigned int m_dwTwoSide = 0;

    std::vector<TEXTURE_DATA> m_vecTextures;
};

typedef std::vector<L3DMaterial> MODEL_MATERIALS;

namespace L3DMaterialPack {
    void LoadFromJson(ID3D11Device* piDevice, MODEL_MATERIALS& InstancePack, const char* szFileName, RUNTIME_MACRO eMacro);
};
