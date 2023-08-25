#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <d3d11.h>

#include "L3DFormat.h"

#include "L3DTexture.h"
#include "L3DMaterialConfig.h"
#include "L3DMaterialDefine.h"

#include "rapidjson/include/rapidjson/document.h"

class L3DMaterialDefine;
class L3DEffect;

struct L3D_STATE_TABLE;

struct ID3DX11EffectPass;
struct ID3DX11EffectConstantBuffer;
struct ID3DX11EffectShaderResourceVariable;

struct MATERIAL_INSTANCE_DATA
{
    char szDefineName[MAX_PATH];
    UINT32 uBlendMode   = 0;
    UINT32 uAlphaRef    = 0;
    UINT32 uAlphaRef2   = 0;
    UINT32 uTwoSideFlag = 0;

    std::unordered_map<std::string, std::string> TextureArray;
};

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

class L3DMaterial
{
public:
    HRESULT Create(ID3D11Device* piDevice, MATERIAL_INSTANCE_DATA& InstanceData, RUNTIME_MACRO eMacro);
    HRESULT ApplyRenderState(ID3D11DeviceContext* pDeviceContext, const L3D_STATE_TABLE* pStateTable);
    HRESULT Apply(ID3D11DeviceContext* pDeviceContext, RENDER_PASS ePass);

    HRESULT CreateIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer** pEffectCB);
    HRESULT SetIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer* pSharedCB);

    void GetRenderStateValue(SKIN_SUBSET_CONST & subsetConst);

    ~L3DMaterial() {}

    BlendMode m_eBlendMode = BLEND_OPAQUE;
private:
    struct PASS_TEXTURE
    {
        ID3DX11EffectShaderResourceVariable* pEffectSRVariable;
        L3DTexture* pTexture;
    };  

    HRESULT _PlaceTextureValue(ID3D11Device* piDevice, std::string sName, std::string sTextureName);

    HRESULT _UpdateTechniques(RENDER_PASS ePass, ID3DX11EffectPass** ppEffectPass);
    HRESULT _UpdateTextures();
    void _UpdateCommonCB();

    L3DEffect*         m_pEffect         = nullptr;
    ID3D11Buffer*      m_piCommonCB      = nullptr;
    L3DMaterialDefine* m_pMaterialDefine = nullptr;


    BOOL m_AlphaTestSwitch = false;
    unsigned int m_dwAlphaRef = 0;
    unsigned int m_dwAlphaRef2 = 0;
    unsigned int m_dwTwoSide = 0;

    std::vector<TEXTURE_DATA> m_vecTextures;
};

class L3DMaterialPack
{
public:
    static void LoadFromJson(ID3D11Device* piDevice, MATERIALS_PACK& InstancePack, const wchar_t* szFileName, RUNTIME_MACRO eMacro);
private:
    static void _LoadInstanceFromJson(const rapidjson::Value& JsonObject, MATERIAL_INSTANCE_DATA& InstanceData);
};
