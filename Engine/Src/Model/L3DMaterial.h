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

struct ID3DX11EffectPass;
struct ID3DX11EffectConstantBuffer;
struct ID3DX11EffectShaderResourceVariable;

struct MATERIAL_INSTANCE_DATA
{
    char szDefineName[MAX_PATH];
    UINT32 uBlendMode;
    UINT32 uAlphaRef;

    std::unordered_map<std::string, std::string> TextureArray;
};

enum class MATERIAL_INDIV_CB
{
    MODELSHARED,
    SUBSET,
};

enum BlendMode
{
    BLEND_Opaque,
    BLEND_Masked,
};

class L3DMaterial
{
public:
    HRESULT Create(ID3D11Device* piDevice, MATERIAL_INSTANCE_DATA& InstanceData, RUNTIME_MACRO eMacro);
    HRESULT Apply(ID3D11DeviceContext* pDeviceContext, RENDER_PASS ePass);

    HRESULT CreateIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer** pEffectCB);
    HRESULT SetIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer* pSharedCB);

    void GetRenderStateValue(BOOL* pEnableAlphaTest, float* pAlphaRef);

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

    L3DMaterialDefine* m_pMaterialDefine = nullptr;
    ID3D11Buffer* m_piCommonCB = nullptr;
    L3DEffect* m_pEffect = nullptr;

    BOOL m_AlphaTestSwitch = false;
    unsigned int m_dwAlphaRef = 0;
    BlendMode m_eBlendMode = BLEND_Opaque;

    std::vector<TEXTURE_DATA> m_vecTextures;
};

class L3DMaterialPack
{
public:
    static void LoadFromJson(ID3D11Device* piDevice, MATERIAL_INSTANCE_PACK& InstancePack, const char* szFileName, RUNTIME_MACRO eMacro);
private:
    static void _LoadInstanceFromJson(rapidjson::Value& JsonObject, MATERIAL_INSTANCE_DATA& InstanceData);
};
