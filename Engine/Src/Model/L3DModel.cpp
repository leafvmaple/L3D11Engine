#include "stdafx.h"

#include "L3DModel.h"
#include "L3DMesh.h"
#include "L3DMaterial.h"

#include "Render/L3DRenderUnit.h"

HRESULT L3DModel::Create(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    RUNTIME_MACRO eStaticMacro = RUNTIME_MACRO_MESH;

    m_pMesh = new L3DMesh;
    BOOL_ERROR_EXIT(m_pMesh);

    hr = m_pMesh->Create(piDevice, szFileName);
    HRESULT_ERROR_EXIT(hr);

    if (m_pMesh->m_dwBoneCount > 0)
        eStaticMacro = RUNTIME_MACRO_SKIN_MESH;

    m_pMaterial = new L3DMaterial;
    BOOL_ERROR_EXIT(m_pMaterial);

    hr = m_pMaterial->Create(piDevice, eStaticMacro);
    HRESULT_ERROR_EXIT(hr);

    m_pRenderUnit = new L3DRenderUnit;
    BOOL_ERROR_EXIT(m_pRenderUnit);

    hr = m_pRenderUnit->Create(m_pMesh->m_Stage, m_pMaterial);
    HRESULT_ERROR_EXIT(hr);

    hr = ResetTransform();
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT L3DModel::ResetTransform()
{
	XMStoreFloat4x4(&m_World, XMMatrixIdentity());
	XMStoreFloat4(&m_Rotation, g_XMIdentityR3);
	XMStoreFloat3(&m_Scale, g_XMOne);
	XMStoreFloat3(&m_Translation, g_XMZero);

    m_pRenderUnit->SetWorldMatrix(m_World);

    return S_OK;
}

HRESULT L3DModel::SetTranslation(const XMFLOAT3& Translation)
{
    m_Translation = Translation;

    UpdateTransFrom();

    return S_OK;
}

HRESULT L3DModel::SetRotation(const XMFLOAT4& Rotation)
{
    m_Rotation = Rotation;

    UpdateTransFrom();

    return S_OK;
}

HRESULT L3DModel::SetScale(const XMFLOAT3& Scale)
{
    m_Scale = Scale;

    UpdateTransFrom();

    return S_OK;
}

void L3DModel::UpdateTransFrom()
{
    XMStoreFloat4x4(&m_World,
        XMMatrixTransformation(
            g_XMZero,
            g_XMIdentityR3,
            XMLoadFloat3(&m_Scale),
            g_XMZero,
            XMLoadFloat4(&m_Rotation),
            XMLoadFloat3(&m_Translation)
        )
    );

    m_pRenderUnit->SetWorldMatrix(m_World);
}
