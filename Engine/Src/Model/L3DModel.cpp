#include "stdafx.h"

#include "L3DModel.h"
#include "L3DMesh.h"
#include "L3DMaterial.h"
#include "L3DTexture.h"
#include "L3DAnimation.h"
#include "L3DSkeleton.h"

#include "IO/LFileReader.h"
#include "Utility/FilePath.h"

#include "FX11/inc/d3dx11effect.h"

#include "Render/L3DRenderUnit.h"

// Temp
#include "Camera/L3DCamera.h"

HRESULT L3DModel::Create(ID3D11Device* piDevice, const char* szFileName)
{
    L3D::PathFormat(szFileName, m_Path);
    m_InitFuncs[m_Path.extension()](piDevice, szFileName);

    return S_OK;
}


void L3DModel::AttachActor(L3DModel* pModel)
{
    m_ChildList.emplace_back(pModel);
    if (m_pSkeleton && pModel->m_p3DMesh)
        m_pSkeleton->BindMesh(pModel->m_p3DMesh);
}


void L3DModel::GetAllModel(std::vector<L3DModel*>& models)
{
    if (m_p3DMesh)
    {
        models.push_back(this);
        return;
    }

    for (const auto& child : m_ChildList)
        child->GetAllModel(models);
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

HRESULT L3DModel::PlayAnimation(const char* szAnimation, ANIMATION_PLAY_TYPE nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    return PlaySplitAnimation(szAnimation, SPLIT_ALL, nPlayType, nPriority);
}


HRESULT L3DModel::PlaySplitAnimation(const char* szAnimation, SPLIT_TYPE nSplitType, ANIMATION_PLAY_TYPE nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    L3DAnimation* pAnimation = nullptr;

    assert(nSplitType == SPLIT_ALL);

    if (!m_p3DAniController[nSplitType])
        m_p3DAniController[nSplitType] = new L3DAnmationController;

    pAnimation = new L3DAnimation;
    pAnimation->LoadFromFile(szAnimation);

    if (m_pSkeleton)
        m_p3DAniController[nSplitType]->SetBoneAniInfo(
            m_pSkeleton->m_nNumBones,
            m_pSkeleton->m_BoneInfo.data(),
            m_pSkeleton->m_uFirsetBaseBoneIndex
        );

    m_p3DAniController[nSplitType]->StartAnimation(pAnimation, nPlayType, nPriority);

    return S_OK;
}


void L3DModel::PrimaryUpdate()
{
    _FrameMove();
    _UpdateAnimation();
}

void L3DModel::UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption)
{
    std::vector<XMMATRIX> matBone;
    MESH_SHARED_CB MeshCB;

    matBone.resize(m_BoneCurMatrix.size());
    for (int i = 0; i < m_BoneCurMatrix.size(); i++)
        matBone[i] = XMMatrixMultiply(m_p3DMesh->m_pL3DBone->m_pBoneInfo->BoneOffset[i], m_BoneCurMatrix[i]);

    MeshCB.MatrixWorld = m_World;

    _UpdateModelSharedConsts(matBone, MeshCB);
    for (int i = 0; i < m_MaterialPack.size(); i++)
        _UpdateSubsetConst(i);
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
    if (m_RenderData.ModelVariables.pCustomMatrixBones)
        m_RenderData.ModelVariables.pCustomMatrixBones->SetRawValue(BoneMatrix.data(), 0, sizeof(XMMATRIX) * BoneMatrix.size());
    if (m_RenderData.ModelVariables.pModelParams)
        m_RenderData.ModelVariables.pModelParams->SetRawValue(&MeshCB, 0, sizeof(MESH_SHARED_CB));
}

__declspec(align(16)) struct SKIN_SUBSET_CONST
{
    XMFLOAT4A   ModelColor;
    BOOL        EnableAlphaTest;
    float       AlphaReference;
};

void L3DModel::_UpdateSubsetConst(unsigned int iSubset)
{
    SKIN_SUBSET_CONST subsetConst;

    m_MaterialPack[iSubset]->GetRenderStateValue(&subsetConst.EnableAlphaTest, &subsetConst.AlphaReference);
    m_RenderData.SubsetCB[iSubset]->SetRawValue(&subsetConst, 0, sizeof(SKIN_SUBSET_CONST));
}


void L3DModel::_UpdateAnimation()
{
    if (m_p3DAniController[SPLIT_ALL])
        m_p3DAniController[SPLIT_ALL]->UpdateAnimation();

    _UpdateBuffer();
}

void L3DModel::_UpdateBuffer()
{
    HRESULT hResult = E_FAIL;
    XMMATRIX* pBoneMatrixAll = nullptr;
    int* pSkeletonIndies = nullptr;

    pBoneMatrixAll = m_p3DAniController[SPLIT_ALL]->GetAnimationInfo()->BoneAni.pBoneMatrix;
    BOOL_SUCCESS_EXIT(!pBoneMatrixAll);

    for (auto& pModel : m_ChildList)
    {
        L3DMesh* pMesh = pModel->m_p3DMesh;
        if (!pMesh)
            continue;

        pSkeletonIndies = g_SkeletonBoneManager.GetData(m_pSkeleton->m_sName, pMesh->m_sName);
        BOOL_ERROR_EXIT(pSkeletonIndies);

        for (int i = 0; i < pMesh->m_dwBoneCount; i++)
        {
            unsigned int nIndex = pSkeletonIndies[i];
            pModel->m_BoneCurMatrix[i] = pBoneMatrixAll[nIndex];
        }
    }

Exit0:
    return;
}

void L3DModel::_FrameMove()
{
    if (m_p3DAniController[SPLIT_ALL])
        m_p3DAniController[SPLIT_ALL]->FrameMove();
}

void L3DModel::_CreateBoneMatrix()
{
    m_BoneCurMatrix = m_p3DMesh->m_BoneMatrix;
}


void L3DModel::_UpdateModelVariablesIndices()
{
    m_RenderData.ModelVariables.pCustomMatrixBones = m_RenderData.piModelSharedCB->GetMemberByName("g_CustomMatrixBones");
    m_RenderData.ModelVariables.pModelParams = m_RenderData.piModelSharedCB->GetMemberByName("g_ModelParams");
}

void L3DModel::_InitMdl(ID3D11Device* piDevice, const char* szFileName)
{
    char szPath[MAX_PATH];
    char szExt[MAX_PATH];
    FILE* f = fopen(szFileName, "r");

    fscanf(f, "%s", szPath);
    _InitSkeletion(piDevice, szPath); // Load .txt

    while (fscanf(f, "%s", szPath) != EOF)
    {
        L3DModel* pMesh = new L3DModel;
        pMesh->Create(piDevice, szPath);
        AttachActor(pMesh);
    }
}


void L3DModel::_InitSkeletion(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;

    m_pSkeleton = new L3DSkeleton;
    BOOL_ERROR_EXIT(m_pSkeleton);

    hr = m_pSkeleton->Create(piDevice, szFileName);
    HRESULT_ERROR_EXIT(hr);

Exit0:
    return;
}

void L3DModel::_InitSingleModel(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;

    char szMaterialName[MAX_PATH];

    m_p3DMesh = new L3DMesh;
    BOOL_ERROR_EXIT(m_p3DMesh);

    hr = m_p3DMesh->Create(piDevice, szFileName);
    HRESULT_ERROR_EXIT(hr);

    strcpy(szMaterialName, szFileName);
    hr = L3D::ReplaceExtName(szMaterialName, ".JsonInspack");
    if (SUCCEEDED(hr))
        _LoadMaterialFromJson(piDevice, szMaterialName);

    _CreateBoneMatrix();
    _InitRenderUnits();

    hr = ResetTransform();
    HRESULT_ERROR_EXIT(hr);

Exit0:
    return;
}

void L3DModel::_InitRenderUnits()
{
    unsigned int nSubsetCount = m_p3DMesh->m_Subset.size();

    m_RenderData.RenderUnits.resize(nSubsetCount);
    m_RenderData.SubsetCB.resize(nSubsetCount);

    for (int i = 0; i < nSubsetCount; i++)
    {
        auto& unit = m_RenderData.RenderUnits[i];
        auto pMaterial = m_MaterialPack[i];

        m_p3DMesh->ApplyMeshSubset(unit.m_IAStage, i);

        if (!i)
        {
            pMaterial->CreateIndividualCB(MATERIAL_INDIV_CB::MODELSHARED, &m_RenderData.piModelSharedCB);
            _UpdateModelVariablesIndices();
        }
        else
        {
            pMaterial->SetIndividualCB(MATERIAL_INDIV_CB::MODELSHARED, m_RenderData.piModelSharedCB);
        }

        pMaterial->CreateIndividualCB(MATERIAL_INDIV_CB::SUBSET, &m_RenderData.SubsetCB[i]);

        unit.m_piModelSharedCB = m_RenderData.piModelSharedCB;
        unit.m_piSubsetSharedCB = m_RenderData.SubsetCB[i];
        unit.m_pMaterial = pMaterial;
    }
}
