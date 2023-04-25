#include "stdafx.h"

#include "L3DModel.h"
#include "L3DMesh.h"
#include "L3DMaterial.h"
#include "L3DTexture.h"

#include "IO/LFileReader.h"
#include "Utility/FilePath.h"

#include "FX11/inc/d3dx11effect.h"

#include "Render/L3DRenderUnit.h"

// Temp
#include "Camera/L3DCamera.h"

HRESULT L3DModel::Create(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    char szMaterialName[MAX_PATH];

    m_p3DMesh = new L3DMesh;
    BOOL_ERROR_EXIT(m_p3DMesh);

    hr = m_p3DMesh->Create(piDevice, szFileName);
    HRESULT_ERROR_EXIT(hr);

    strcpy(szMaterialName, szFileName);
    hr = L3D::FormatExtName(szMaterialName, ".JsonInspack");
    if (SUCCEEDED(hr))
        _LoadMaterialFromJson(piDevice, szMaterialName);

    _CreateBoneMatrix();
    _InitRenderUnits();

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


void L3DModel::UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption)
{
    std::vector<XMMATRIX> matBone;
    MESH_SHARED_CB MeshCB;

    matBone.resize(m_BoneCurMatrix.size());
    for (int i = 0; i < m_BoneCurMatrix.size(); i++)
        matBone[i] = XMMatrixMultiply(m_p3DMesh->GetMesh().BoneMatrix[i], m_BoneCurMatrix[i]);

    MeshCB.MatrixWorld = m_World;

    _UpdateModelSharedConsts(matBone, MeshCB);
}


void L3DModel::GetRenderUnit(std::vector<L3DRenderUnit*>& RenderQueue)
{
    // TODO
    for (auto& unit : m_RenderData.RenderUnits)
        RenderQueue.push_back(&unit);
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
}

void L3DModel::_LoadMaterialFromJson(ID3D11Device* piDevice, const char* szFileName)
{
    RUNTIME_MACRO eMacro = RUNTIME_MACRO_MESH;
    if (m_p3DMesh->m_dwBoneCount > 0)
        eMacro = RUNTIME_MACRO_SKIN_MESH;

    L3DMaterialPack::LoadFromJson(piDevice, m_MaterialPack, szFileName, eMacro);
}

void L3DModel::_UpdateModelSharedConsts(std::vector<XMMATRIX>& BoneMatrix, const MESH_SHARED_CB& MeshCB)
{
    m_RenderData.ModelVariables.pCustomMatrixBones->SetRawValue(BoneMatrix.data(), 0, sizeof(XMMATRIX) * BoneMatrix.size());
    m_RenderData.ModelVariables.pModelParams->SetRawValue(&MeshCB, 0, sizeof(MESH_SHARED_CB));
}

void L3DModel::_CreateBoneMatrix()
{
    m_BoneCurMatrix = m_p3DMesh->GetMesh().BoneMatrix;
}


void L3DModel::_UpdateModelVariablesIndices()
{
    m_RenderData.ModelVariables.pCustomMatrixBones = m_RenderData.piModelSharedCB->GetMemberByName("g_CustomMatrixBones");
    m_RenderData.ModelVariables.pModelParams = m_RenderData.piModelSharedCB->GetMemberByName("g_ModelParams");
}

void L3DModel::_InitRenderUnits()
{
    auto& mesh = m_p3DMesh->GetMesh();

    m_RenderData.RenderUnits.resize(mesh.uSubsetCount);
    m_RenderData.SubsetCB.resize(mesh.uSubsetCount);

    for (int i = 0; i < mesh.uSubsetCount; i++)
    {
        auto& unit = m_RenderData.RenderUnits[i];
        auto pMaterial = m_MaterialPack[i];

        m_p3DMesh->ApplyMeshSubset(unit.m_IAStage, i);

        if (!i)
        {
            pMaterial->CreateIndividualCB(MATERIAL_INDIV_CB::MODELSHARED, &m_RenderData.piModelSharedCB);
            _UpdateModelVariablesIndices();
        }

        pMaterial->CreateIndividualCB(MATERIAL_INDIV_CB::SUBSET, &m_RenderData.SubsetCB[i]);

        unit.m_piModelSharedCB = m_RenderData.piModelSharedCB;
        unit.m_piSubsetSharedCB = m_RenderData.SubsetCB[i];
        unit.m_pMaterial = pMaterial;
    }
}
