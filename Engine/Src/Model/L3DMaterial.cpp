#include "stdafx.h"

#include <d3dcompiler.h>
#include <atlconv.h>
#include <string>

#include "L3DMaterial.h"
#include "L3DEffect.h"

#include "L3DInterface.h"
#include "L3DMaterialConfig.h"
#include "L3DMaterialDefine.h"

#include "FX11/inc/d3dx11effect.h"


struct RENDER_PASS_TABLE
{
    RENDER_PASS ePass;
    LPCSTR      szTechniqueName;
    unsigned    uPassSlot;
};

static RENDER_PASS_TABLE g_MaterialPassDeclares[] = {
    {RENDER_PASS::COLOR, "Color", 0},
};

HRESULT L3DMaterial::Create(ID3D11Device* piDevice, MATERIAL_INSTANCE_DATA& InstanceData)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    m_pMaterialDefine = new L3DMaterialDefine;
    BOOL_ERROR_EXIT(m_pMaterialDefine);

    hr = m_pMaterialDefine->Create(InstanceData.szDefineName);
    HRESULT_ERROR_EXIT(hr);

    hr = m_pMaterialDefine->GetTextureVariables(piDevice, m_vecTextures);
    HRESULT_ERROR_EXIT(hr);

    for (auto iter = InstanceData.TextureArray.cbegin(), iend = InstanceData.TextureArray.cend(); iter != iend; ++iter)
    {
        hr = _PlaceTextureValue(piDevice, iter->first, iter->second);
        HRESULT_ERROR_EXIT(hr);
    }

    m_pEffect = new L3DEffect;
    BOOL_ERROR_EXIT(m_pEffect);

    hr = m_pEffect->Create(piDevice, InstanceData.eMacro);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT L3DMaterial::Apply(ID3D11DeviceContext* pDeviceContext, RENDER_PASS ePass)
{
    HRESULT hr = E_FAIL;

    ID3DX11EffectPass* pEffectPass = nullptr;

    // _GetRenderPass
    hr = _UpdateTechniques(ePass, &pEffectPass);
    HRESULT_ERROR_EXIT(hr);

    hr = _UpdateTextures();
    HRESULT_ERROR_EXIT(hr);

    hr = pEffectPass->Apply(0, pDeviceContext);
    HRESULT_ERROR_EXIT(hr);

Exit0:
    return S_OK;
}


HRESULT L3DMaterial::CreateIndividualCB(MATERIAL_INDIV_CB eCBType, ID3DX11EffectConstantBuffer** pEffectCB)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    switch (eCBType)
    {
    case MATERIAL_INDIV_CB::MODELSHARED:
    {
        *pEffectCB = m_pEffect->GetConstantBufferByName("ModelSharedParam");
        BOOL_ERROR_EXIT(*pEffectCB);
    }
        break;
    default:
        break;
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMaterial::_PlaceTextureValue(ID3D11Device* piDevice, std::string sName, std::string sTextureName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    for (auto it = m_vecTextures.begin(); it != m_vecTextures.end(); it++)
    {
        if (sName == it->sRepresentName)
        {
            SAFE_DELETE(it->pTexture);

            it->pTexture = new L3DTexture;
            BOOL_ERROR_EXIT(it->pTexture);

            hr = it->pTexture->Create(piDevice, sTextureName.c_str());
            HRESULT_ERROR_EXIT(hr);
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT L3DMaterial::_UpdateTechniques(RENDER_PASS ePass, ID3DX11EffectPass** ppEffectPass)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    ID3DX11EffectTechnique* pEffectTechnique = nullptr;
    ID3DX11EffectPass* pEffectPass = nullptr;

    uint32_t nPass = static_cast<uint32_t>(ePass);

    RENDER_PASS_TABLE& passTable = g_MaterialPassDeclares[nPass];

    pEffectTechnique = m_pEffect->GetTechniqueByName(passTable.szTechniqueName);
    BOOL_ERROR_EXIT(pEffectTechnique);

    pEffectPass = pEffectTechnique->GetPassByIndex(nPass);
    BOOL_ERROR_EXIT(pEffectPass);

    *ppEffectPass = pEffectPass;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMaterial::_UpdateTextures()
{
    HRESULT hr = E_FAIL;

    std::vector<EFFECT_SHADER> EffectShader;

    m_pEffect->GetShader(EffectShader);

    for (auto itEffect : EffectShader)
    {
        for (auto it : m_vecTextures)
        {
            if (it.sRegisterName == itEffect.name)
            {
                hr = it.pTexture->Apply(itEffect.pSRVaraible);
                HRESULT_ERROR_EXIT(hr);
            }
        }
    }

Exit0:
    return S_OK;
}
