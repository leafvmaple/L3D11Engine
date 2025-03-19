#include "stdafx.h"

#include <d3dcompiler.h>
#include <atlconv.h>
#include <string>
#include <condition_variable>

#include "L3DMaterial.h"
#include "L3DEffect.h"
#include "State/L3DState.h"

#include "L3DInterface.h"
#include "L3DMaterialConfig.h"

#include "IMaterial.h"

#include "IO/LFileReader.h"

#include "Render/L3DMaterialSystem.h"

#include "Utility/FilePath.h"

#include "FX11/inc/d3dx11effect.h"


static struct RENDER_PASS_TABLE {
    RENDER_PASS ePass;
    const char* szTechniqueName;
    unsigned    uPassSlot = 0;
} g_MaterialPassDeclares[] = {
    { RENDER_PASS::COLOR, "Color"},
    { RENDER_PASS::COLORSOFTMASK, "ColorSoftMask"}
};

static std::unordered_map<MATERIAL_INDIV_CB, const char*> g_MaterialCBMap = {
    { MATERIAL_INDIV_CB::SUBSET, "SubsetConstParam" },
    { MATERIAL_INDIV_CB::MODELSHARED, "ModelSharedParam" }
};

HRESULT L3DMaterial::Create(ID3D11Device* piDevice, const MATERIAL_SOURCE& source, RUNTIME_MACRO eMacro)
{
    REFER_MATERIAL_DESC desc{ source.Define.szName };
    REFER_MATERIAL_SOURCE configSource{};

    LoadReferMaterial(&desc, &configSource);

    m_vecTextures.clear();
    m_vecTextures.reserve(configSource.nParam);

    for (int i = 0; i < configSource.nParam; i++)
    {
        auto& param = configSource.pParam[i];

        L3DTexture* pTexture = new L3DTexture;

        if (FAILED(pTexture->Create(piDevice, param.szValue)))
            SAFE_DELETE(pTexture);

        m_vecTextures.push_back({ param.szName, param.szRegister, pTexture });
    }

    m_eBlendMode  = static_cast<BlendMode>(source.nBlendMode);
    m_dwAlphaRef  = source.nAlphaRef;
    m_dwAlphaRef2 = source.nAlphaRef2;
    m_dwTwoSide   = source.nTwoSideFlag;

    m_AlphaTestSwitch = (m_eBlendMode == BLEND_MASKED || m_eBlendMode == BLEND_SOFTMASKED);

    for (int i = 0; i < source.nTexture; i++)
        _PlaceTextureValue(piDevice, source.pTexture[i].szName, source.pTexture[i].szValue);

    m_pEffect = new L3DEffect;
    m_pEffect->Create(piDevice, configSource.szShaderName, eMacro);

    return S_OK;
}

HRESULT L3DMaterial::ApplyRenderState(ID3D11DeviceContext* pDeviceContext, const std::shared_ptr<L3D_STATE_TABLE>& pStateTable)
{
    RASTERIZER_STATE_ID eRasterizer = L3D_RASTERIZER_STATE_CULL_CLOCKWISE;

    if (m_dwTwoSide > 0)
        eRasterizer = L3D_RASTERIZER_STATE_CULL_NONE;
    pDeviceContext->RSSetState(pStateTable->Rasterizer[eRasterizer]);

    return S_OK;
}

HRESULT L3DMaterial::Apply(ID3D11DeviceContext* pDeviceContext, RENDER_PASS ePass)
{
    ID3DX11EffectPass* pEffectPass = nullptr;

    _UpdateCommonCB();
    _UpdateTechniques(ePass, &pEffectPass); // In _GetRenderPass
    _UpdateTextures();

    pEffectPass->Apply(0, pDeviceContext);

Exit0:
    return S_OK;
}


void L3DMaterial::SetTexture(const char* szName, L3DTexture* pTexture)
{
    m_pEffect->SetTexture(szName, pTexture);
}

HRESULT L3DMaterial::CreateIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer** pEffectCB)
{
    HRESULT hResult = E_FAIL;

    BOOL_ERROR_EXIT(m_pEffect);
    assert(g_MaterialCBMap.find(eCBType) != g_MaterialCBMap.end());

    *pEffectCB = m_pEffect->GetConstantBufferByName(g_MaterialCBMap[eCBType]);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMaterial::SetIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer* pSharedCB)
{
    HRESULT hResult = E_FAIL;
    ID3DX11EffectConstantBuffer* pEffectCB = nullptr;
    ID3D11Buffer* pBuffer = nullptr;

    BOOL_ERROR_EXIT(m_pEffect);
    assert(g_MaterialCBMap.find(eCBType) != g_MaterialCBMap.end());

    pEffectCB = m_pEffect->GetConstantBufferByName(g_MaterialCBMap[eCBType]);

    pSharedCB->GetConstantBuffer(&pBuffer);
    pEffectCB->SetConstantBuffer(pBuffer);

    hResult = S_OK;
Exit0:
    return hResult;
}


void L3DMaterial::GetRenderStateValue(SKIN_SUBSET_CONST& subsetConst)
{
    subsetConst.EnableAlphaTest = m_AlphaTestSwitch;
    subsetConst.AlphaReference = m_dwAlphaRef / 255.f;
    subsetConst.AlphaReference2 = m_dwAlphaRef2 / 255.f;
}

HRESULT L3DMaterial::_PlaceTextureValue(ID3D11Device* piDevice, std::string sName, std::string sTextureName)
{
    for (auto& texture : m_vecTextures)
    {
        if (sName == texture.sRepresentName)
        {
            SAFE_DELETE(texture.pTexture);

            texture.pTexture = new L3DTexture;
            texture.pTexture->Create(piDevice, sTextureName.c_str());
        }
    }

    return S_OK;
}


HRESULT L3DMaterial::_UpdateTechniques(RENDER_PASS ePass, ID3DX11EffectPass** ppEffectPass)
{
    ID3DX11EffectTechnique* pEffectTechnique = nullptr;

    uint32_t nPass = static_cast<uint32_t>(ePass);
    RENDER_PASS_TABLE& passTable = g_MaterialPassDeclares[nPass];

    pEffectTechnique = m_pEffect->GetTechniqueByName(passTable.szTechniqueName);
    *ppEffectPass = pEffectTechnique->GetPassByIndex(0); // p0

    return S_OK;
}

HRESULT L3DMaterial::_UpdateTextures()
{
    std::unordered_map <std::string, ID3DX11EffectShaderResourceVariable*> EffectShader;

    m_pEffect->GetTextures(EffectShader);

    for (auto it : m_vecTextures)
    {
        if (EffectShader.contains(it.sRegisterName))
            it.pTexture->Apply(EffectShader[it.sRegisterName]);
    }

    return S_OK;
}


void L3DMaterial::_UpdateCommonCB()
{
    auto& CommonCBs = g_pMaterialSystem->GetCommonCBList();
    for (auto cb : CommonCBs)
        m_pEffect->GetConstantBufferByRegister(cb.first)->SetConstantBuffer(cb.second);
}

void L3DMaterialPack::LoadFromJson(ID3D11Device* piDevice, MODEL_MATERIALS& InstancePack, const char* szFileName, RUNTIME_MACRO eMacro)
{
    MODEL_MATERIAL_DESC desc;
    MODEL_MATERIAL_SOURCE source;

    desc.szFileName = szFileName;
    LoadModelMaterial(&desc, &source);

    assert(source.nLOD > 0 && source.pLOD[0].nGroup > 0);

    const auto& Group = source.pLOD[0].pGroup[0];
    InstancePack.resize(Group.nSubset);
#pragma omp parallel for
    for (int i = 0; i < Group.nSubset; i++)
    {
        const auto& Subset = Group.pSubset[i];
        auto& Instance = InstancePack[i];

        Instance.Create(piDevice, Subset, eMacro);
    }
}
