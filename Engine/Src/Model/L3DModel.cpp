#include "stdafx.h"

#include "L3DModel.h"
#include "L3DMesh.h"
#include "L3DMaterial.h"
#include "L3DTexture.h"
#include "L3DAnimation.h"
#include "L3DSkeleton.h"

#include "Flexible/L3DFlexible.h"

#include "IO/LFileReader.h"
#include "Utility/FilePath.h"

#include "FX11/inc/d3dx11effect.h"

#include "Render/L3DRenderUnit.h"

// Temp
#include "Camera/L3DCamera.h"

L3DModel::~L3DModel() = default;

bool L3DModel::Create(ID3D11Device* piDevice, const char* szFileName)
{
    m_Path = std::filesystem::absolute(szFileName);

    ResetTransform();
    CHECK_BOOL(m_InitFuncs[m_Path.lower_extension()](piDevice, szFileName));

    return true;
}


void L3DModel::AttachActor(L3DModel* pModel)
{
    pModel->m_BindInfo.eBindType = ATTACH_TO_ACTOR;
    pModel->m_BindInfo.extraInfo.pModel = this;

    m_ChildList.emplace_back(pModel);
    if (m_pSkeleton && pModel->m_p3DMesh)
        m_pSkeleton->BindMesh(pModel->m_p3DMesh.get());
}


void L3DModel::BindToSocket(L3DModel* pModel, const char* szSocketName)
{
    m_BindInfo.eBindType = BIND_TO_SOCKET;
    m_BindInfo.sBindTarget = szSocketName;

    if (pModel->_FindSocket(szSocketName, m_BindInfo.extraInfo));
        m_BindInfo.extraInfo.pModel->m_ChildList.emplace_back(this);
}

void L3DModel::GetAllModel(std::vector<L3DModel*>& models)
{
    if (m_p3DMesh)
        models.push_back(this);

    for (const auto& child : m_ChildList)
        child->GetAllModel(models);
}


void L3DModel::GetWorldMatrix(XMFLOAT4X4* pMatrix)
{
    *pMatrix = m_World;
}

void L3DModel::GetSocketMatrix(XMFLOAT4X4* pMatrix, int nSocketIndex)
{
    XMMATRIX mSocketCurMatrix;
    SOCKETINFO& Socket = m_p3DMesh->m_pL3DBone->m_Socket[nSocketIndex];

    mSocketCurMatrix = XMMatrixMultiply(XMLoadFloat4x4(&Socket.mOffset), m_BoneCurMatrix[Socket.uParentBoneIndex]);
    XMStoreFloat4x4(pMatrix, XMMatrixMultiply(mSocketCurMatrix, XMLoadFloat4x4(&m_World)));
}

void L3DModel::AttachModel(ILModel* pModel)
{
    L3DModel* p3DModel = dynamic_cast<L3DModel*>(pModel);
    AttachActor(p3DModel);

Exit0:
    return;
}

void L3DModel::BindToSocket(ILModel* pModel, const char* szSocketName)
{
    L3DModel* p3DModel = dynamic_cast<L3DModel*>(pModel);

    BindToSocket(p3DModel, szSocketName);
    UpdateTransform();

Exit0:
    return;
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

    UpdateTransform();

    return S_OK;
}

HRESULT L3DModel::SetRotation(const XMFLOAT4& Rotation)
{
    m_Rotation = Rotation;

    UpdateTransform();

    return S_OK;
}

HRESULT L3DModel::SetScale(const XMFLOAT3& Scale)
{
    m_Scale = Scale;

    UpdateTransform();

    return S_OK;
}

HRESULT L3DModel::PlayAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    return PlaySplitAnimation(szAnimation, SPLIT_ALL, nPlayType, nPriority);
}


HRESULT L3DModel::PlaySplitAnimation(const char* szAnimation, SPLIT_TYPE nSplitType, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    L3DAnimation* pAnimation = nullptr;

    assert(nSplitType == SPLIT_ALL);

    if (!m_p3DAniController[nSplitType])
        m_p3DAniController[nSplitType] = std::make_unique<L3DAnimationController>();

    if (m_pSkeleton)
        // 将骨架的骨骼信息传递给动画控制器
        m_p3DAniController[nSplitType]->SetBoneAniInfo(
            m_pSkeleton->m_nNumBones,
            &m_pSkeleton->m_BoneInfo,
            m_pSkeleton->m_uFirsetBaseBoneIndex
        );

    m_p3DAniController[nSplitType]->StartAnimation(szAnimation, nPlayType, nPriority);

    return S_OK;
}


void L3DModel::PrimaryUpdate()
{
    _UpdateTransform();
    _FrameMove();
    _UpdateAnimation();

    if (m_pFlexible)
        m_pFlexible->Update(m_BoneCurMatrix, m_World);

    std::ranges::for_each(m_ChildList, [](auto& child) { child->PrimaryUpdate(); });
}

void L3DModel::UpdateTransform()
{
    _UpdateTransform();

    for (auto& child : m_ChildList)
        child->UpdateTransform();
}

void L3DModel::UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption)
{
    std::vector<XMMATRIX> matBone;
    MESH_SHARED_CB MeshCB;

    // matBone是骨骼相对于初始状态(T Pose)的偏移值
    matBone.resize(m_BoneCurMatrix.size());
    for (int i = 0; i < m_BoneCurMatrix.size(); i++)
        matBone[i] = XMMatrixMultiply(m_p3DMesh->m_pL3DBone->m_BoneOffset[i], m_BoneCurMatrix[i]);

    MeshCB.MatrixWorld = m_World;

    _UpdateModelSharedConsts(matBone, MeshCB);
    for (int i = 0; i < m_MaterialPack.size(); i++)
        _UpdateSubsetConst(i);
}


void L3DModel::GetRenderUnit(SCENE_RENDER_QUEUES& RenderQueue)
{
    // TODO
    for (auto& unit : m_RenderData.RenderUnits)
    {
        RenderQueue.GBufferQueue.push_back(&unit);
        if (unit.m_pMaterial->m_eBlendMode == BLEND_SOFTMASKED)
            RenderQueue.TransparencyQueue.push_back(&unit);
    }
}

void L3DModel::_UpdateTransform()
{
    if (m_BindInfo.eBindType == BIND_TO_SOCKET)
    {
        _UpdateTransformBindToSocket();
    }
    else if (m_BindInfo.eBindType == ATTACH_TO_ACTOR)
    {
        _UpdateTransformAttachToActor();
    }
    else
    {
        _UpdateTransformDefault();
    }
}

void L3DModel::_UpdateTransformBindToSocket()
{
    m_BindInfo.extraInfo.pModel->GetSocketMatrix(&m_World, m_BindInfo.extraInfo.nBindIndex);
}

void L3DModel::_UpdateTransformAttachToActor()
{
    m_BindInfo.extraInfo.pModel->GetWorldMatrix(&m_World);
}

void L3DModel::_UpdateTransformDefault()
{
    XMStoreFloat4x4(&m_World, XMMatrixTransformation(
        g_XMZero,
        g_XMIdentityR3,
        XMLoadFloat3(&m_Scale),
        g_XMZero,
        XMLoadFloat4(&m_Rotation),
        XMLoadFloat3(&m_Translation)
    ));
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

void L3DModel::_UpdateSubsetConst(unsigned int iSubset)
{
    SKIN_SUBSET_CONST subsetConst;

    m_MaterialPack[iSubset].GetRenderStateValue(subsetConst);
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
    std::vector<XMMATRIX>* pBoneMatrix = nullptr;
    std::vector<int>* pSkeletonIndies = nullptr;

    BOOL_SUCCESS_EXIT(!m_p3DAniController[SPLIT_ALL]);

    pBoneMatrix = m_p3DAniController[SPLIT_ALL]->m_UpdateAniInfo.BoneAni.pBoneMatrix;
    BOOL_SUCCESS_EXIT(!pBoneMatrix);

    for (auto& pModel : m_ChildList)
    {
        const auto const pMesh = pModel->m_p3DMesh.get();
        if (!pMesh)
            continue;

        pSkeletonIndies = g_SkeletonBoneManager.GetData(m_pSkeleton->m_sName, pMesh->m_sName);
        BOOL_ERROR_EXIT(pSkeletonIndies);

        for (UINT nBoneIndex = 0; nBoneIndex < pMesh->m_dwBoneCount; nBoneIndex++)
        {
            unsigned int nSkeletonIndex = (*pSkeletonIndies)[nBoneIndex];
            if (nSkeletonIndex != -1)
                pModel->m_BoneCurMatrix[nBoneIndex] = (*pBoneMatrix)[nSkeletonIndex];
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
    // 初始化时使用初始状态的骨骼矩阵，保证无动作状态下的顶点位置为初始状态
    m_BoneCurMatrix = m_p3DMesh->m_BoneMatrix;
}


void L3DModel::_UpdateModelVariablesIndices(ID3DX11EffectConstantBuffer* piModelSharedCB)
{
    m_RenderData.ModelVariables.pCustomMatrixBones = piModelSharedCB->GetMemberByName("g_CustomMatrixBones");
    m_RenderData.ModelVariables.pModelParams = piModelSharedCB->GetMemberByName("g_ModelParams");
}

// _InitMdl
bool L3DModel::_InitModel(ID3D11Device* piDevice, const char* szFileName)
{
    char szPath[MAX_PATH];
    FILE* f = fopen(szFileName, "r");

    CHECK_BOOL(fscanf(f, "%s", szPath) != EOF);
    CHECK_BOOL(_InitSkeletion(piDevice, szPath[0] == '\\' ? szPath + 1 : szPath)); // Load .txt

    while (fscanf(f, "%s", szPath) != EOF)
    {
        L3DModel* pMesh = new L3DModel;
        pMesh->Create(piDevice, szPath[0] == '\\' ? szPath + 1 : szPath);

        AttachActor(pMesh);
    }

    return true;
}


bool L3DModel::_InitSkeletion(ID3D11Device* piDevice, const char* szFileName)
{
    m_pSkeleton = std::make_unique<L3DSkeleton>();
    CHECK_BOOL(m_pSkeleton->Create(piDevice, szFileName));

    return true;
}

// _InitSingleModel
bool L3DModel::_InitMesh(ID3D11Device* piDevice, const char* szFileName)
{
    auto materialPath = m_Path;
    materialPath.replace_extension(".JsonInspack");

    m_p3DMesh = std::make_unique<L3DMesh>();
    CHECK_BOOL(m_p3DMesh->Create(piDevice, m_Path.string().c_str()));

    if (std::filesystem::exists(materialPath))
        _LoadMaterialFromJson(piDevice, materialPath.string().c_str());

    _CreateBoneMatrix();
    _InitRenderUnits();

    m_pFlexible = std::make_unique<L3DFlexible>();
    m_pFlexible->Init(m_p3DMesh.get());

Exit0:
    return true;
}

void L3DModel::_InitRenderUnits()
{
    ID3DX11EffectConstantBuffer* piModelSharedCB = nullptr;
    unsigned int nSubsetCount = m_p3DMesh->m_Subset.size();

    m_RenderData.RenderUnits.resize(nSubsetCount);
    m_RenderData.SubsetCB.resize(nSubsetCount);

    for (int i = 0; i < nSubsetCount; i++)
    {
        auto& unit = m_RenderData.RenderUnits[i];
        auto& material = m_MaterialPack[i];

        m_p3DMesh->ApplyMeshSubset(unit.m_IAStage, i);

        if (!i)
        {
            material.CreateIndividualCB(MATERIAL_INDIV_CB::MODELSHARED, &piModelSharedCB);
            _UpdateModelVariablesIndices(piModelSharedCB);
        }
        else
        {
            material.SetIndividualCB(MATERIAL_INDIV_CB::MODELSHARED, piModelSharedCB);
        }
        material.CreateIndividualCB(MATERIAL_INDIV_CB::SUBSET, &m_RenderData.SubsetCB[i]);

        unit.m_pMaterial = &material;
    }
}

bool L3DModel::_FindSocket(const L3D::lower_string& sockeName, L3D_BIND_EXTRA_INFO& BindExtraInfo)
{
    if (m_p3DMesh)
    {
        for (int i = 0; i < m_p3DMesh->m_pL3DBone->m_Socket.size(); i++)
        {
            const auto& socket = m_p3DMesh->m_pL3DBone->m_Socket[i];
            if (sockeName == socket.sSocketName)
            {
                BindExtraInfo.pModel = this;
                BindExtraInfo.nBindIndex = i;
                return true;
            }
        }
    }

    for (const auto& child : m_ChildList)
        if (child->_FindSocket(sockeName, BindExtraInfo))
            return true;

    return false;
}
