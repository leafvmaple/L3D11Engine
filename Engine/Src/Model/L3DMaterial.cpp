#include "stdafx.h"

#include <d3dcompiler.h>
#include <atlconv.h>
#include <string>

#include "L3DMaterial.h"
#include "L3DEffect.h"
#include "State/L3DState.h"

#include "L3DInterface.h"
#include "L3DMaterialConfig.h"
#include "L3DMaterialDefine.h"

#include "IMaterial.h"

#include "IO/LFileReader.h"

#include "Render/L3DMaterialSystem.h"

#include "FX11/inc/d3dx11effect.h"


struct RENDER_PASS_TABLE
{
    RENDER_PASS ePass;
    const char* szTechniqueName;
    unsigned    uPassSlot;
};

static RENDER_PASS_TABLE g_MaterialPassDeclares[] = {
    {RENDER_PASS::COLOR, "Color", 0},
    {RENDER_PASS::COLORSOFTMASK, "ColorSoftMask", 0}
};

HRESULT L3DMaterial::Create(ID3D11Device* piDevice, const MATERIAL_SOURCE_SUBSET& Subset, RUNTIME_MACRO eMacro)
{
    m_pMaterialDefine = new L3DMaterialDefine;
    m_pMaterialDefine->Create(Subset.Define.szName);
    m_pMaterialDefine->GetTextureVariables(piDevice, m_vecTextures);

    m_eBlendMode  = (BlendMode)Subset.nBlendMode;
    m_dwAlphaRef  = Subset.nAlphaRef;
    m_dwAlphaRef2 = Subset.nAlphaRef2;
    m_dwTwoSide   = Subset.nTwoSideFlag;

    m_AlphaTestSwitch = (m_eBlendMode == BLEND_MASKED || m_eBlendMode == BLEND_SOFTMASKED);

    for (int i = 0; i < Subset.nTexture; i++)
        _PlaceTextureValue(piDevice, Subset.pTexture[i].szName, Subset.pTexture[i].szValue);

    m_pEffect = new L3DEffect;
    m_pEffect->Create(piDevice, m_pMaterialDefine->m_szShaderName, eMacro);

    return S_OK;
}


HRESULT L3DMaterial::ApplyRenderState(ID3D11DeviceContext* pDeviceContext, const L3D_STATE_TABLE* pStateTable)
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


HRESULT L3DMaterial::CreateIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer** pEffectCB)
{
    HRESULT hResult = E_FAIL;

    BOOL_ERROR_EXIT(m_pEffect);

    switch (eCBType)
    {
    case MATERIAL_INDIV_CB::SUBSET:
    {
        *pEffectCB = m_pEffect->GetConstantBufferByName("SubsetConstParam");
        break;
    }
    case MATERIAL_INDIV_CB::MODELSHARED:
    {
        *pEffectCB = m_pEffect->GetConstantBufferByName("ModelSharedParam");
        break;
    }
    default:
        break;
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMaterial::SetIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer* pSharedCB)
{
    HRESULT hResult = E_FAIL;
    ID3DX11EffectConstantBuffer* pCB = nullptr;
    ID3D11Buffer* pBuffer = nullptr;

    BOOL_ERROR_EXIT(m_pEffect);

    switch (eCBType)
    { 
    case MATERIAL_INDIV_CB::SUBSET:
    {
        pCB = m_pEffect->GetConstantBufferByName("SubsetConstParam");
        break;
    }
    case MATERIAL_INDIV_CB::MODELSHARED:
    {
        pCB = m_pEffect->GetConstantBufferByName("ModelSharedParam");
        break;
    }
    default:
        break;
    }

    pSharedCB->GetConstantBuffer(&pBuffer);
    pCB->SetConstantBuffer(pBuffer);

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
    std::vector<EFFECT_SHADER> EffectShader;

    m_pEffect->GetShader(EffectShader);

    for (auto itEffect : EffectShader)
    {
        for (auto it : m_vecTextures)
        {
            if (it.sRegisterName == itEffect.name)
                it.pTexture->Apply(itEffect.pSRVaraible);
        }
    }

    return S_OK;
}


void L3DMaterial::_UpdateCommonCB()
{
    auto& CommonCB = g_pMaterialSystem->GetCommonCBList();
    for (auto cb : CommonCB)
        m_pEffect->GetConstantBufferByRegister(cb.first)->SetConstantBuffer(cb.second);
}

void L3DMaterialPack::LoadFromJson(ID3D11Device* piDevice, MATERIALS_PACK& InstancePack, const wchar_t* szFileName, RUNTIME_MACRO eMacro)
{
    MATERIAL_DESC desc;
    MATERIAL_SOURCE* pSource = nullptr;

    desc.szFileName = szFileName;
    LoadMaterial(&desc, pSource);

    InstancePack.resize(pSource->nLOD);
#pragma omp parallel for
    for (int i = 0; i < pSource->nLOD; i++)
    {
        const auto& LOD = pSource->pLOD[i];
        InstancePack[i].resize(LOD.nGroup);
        for (int j = 0; j < LOD.nGroup; j++)
        {
            const auto& Group = LOD.pGroup[j];
            InstancePack[i][j].resize(Group.nSubset);
            for (int k = 0; k < Group.nSubset; k++)
            {
                const auto& Subset = Group.pSubset[k];
                auto& Instance = InstancePack[i][j][k];

                Instance.Create(piDevice, Subset, eMacro);
            }
        }
    }

    pSource->Release();
}

