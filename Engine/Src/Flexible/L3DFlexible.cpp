#include "stdafx.h"
#include "L3DFlexible.h"

#include "Model/L3DMesh.h"
#include "Model/L3DBone.h"

// ∂¡»°»·ÃÂπ«˜¿
void L3DFlexible::InitFromMesh(L3DMesh* pMesh)
{
    for (int i = 0; i < pMesh->m_dwBoneCount; i++) 
    {
        const auto& boneInfo = pMesh->m_pL3DBone->m_BoneInfo[i];
        if (boneInfo.sBoneName.IsFlexibleBone())
        {
            int nParentIndex = boneInfo.uParentIndex;
            const auto& parentInfo = pMesh->m_pL3DBone->m_BoneInfo[nParentIndex];
            FLEXIBLE_BONE* pDriverBone = nullptr;

            auto it = m_vecDriverBone.find(nParentIndex);
            if (it == m_vecDriverBone.end())
                it = m_vecDriverBone.emplace(nParentIndex, FLEXIBLE_BONE { nParentIndex, parentInfo.sBoneName }).first;
            pDriverBone = &it->second;

            const auto& boneOffset = pMesh->m_pL3DBone->m_BoneOffset[nParentIndex];
            XMStoreFloat4x4(&pDriverBone->InitPose, XMMatrixInverse(nullptr, boneOffset));
            pDriverBone->WorldPose = pDriverBone->InitPose;

            _AppendBoneOfChainFromMesh(pMesh, pDriverBone, i);
        }
    }
}

void L3DFlexible::Update(std::vector<XMMATRIX>& BoneMatrix, const XMFLOAT4X4& mWorld)
{
    XMMATRIX xmWorld;
    XMMATRIX xmWorldInvert;

    xmWorld = XMLoadFloat4x4(&mWorld);
    xmWorldInvert = XMMatrixInverse(nullptr, xmWorld);

    // TODO
    for (auto& it : m_vecDriverBone)
         XMStoreFloat4x4(&it.second.WorldPose, XMMatrixMultiply(BoneMatrix[it.second.nIndex], xmWorld));

    // if (m_FlexibleBodyUpdateState == KG3D_FLEX_UPDATE_FROM_ANI)
    _UpdateFromAni();

    for (auto& bone : m_vecNormalBone)
        BoneMatrix[bone.nIndex] = XMMatrixMultiply(XMLoadFloat4x4(&bone.WorldPose), xmWorldInvert);
}

void L3DFlexible::_UpdateFromAni()
{
    XMMATRIX xmInvert;
    XMMATRIX xmMovement;

    for (auto driver : m_vecDriverBone)
    {
        FLEXIBLE_BONE& driveBone = driver.second;

        xmInvert = XMMatrixInverse(nullptr, XMLoadFloat4x4(&driveBone.InitPose));
        xmMovement = XMMatrixMultiply(xmInvert, XMLoadFloat4x4(&driveBone.WorldPose));

        for (auto child : driveBone.vecBoneChain)
        {
            FLEXIBLE_BONE& normalBone = m_vecNormalBone[child];
            XMStoreFloat4x4(&(normalBone.WorldPose), XMLoadFloat4x4(&(normalBone.InitPose)) * xmMovement);
        }
    }

    m_FlexibleBodyUpdateState = KG3D_FLEX_UPDATE_FROM_PHYSX;
}

void L3DFlexible::_AppendBoneOfChainFromMesh(L3DMesh* pMesh, FLEXIBLE_BONE* pDriverBone, int nIndex)
{
    const auto& boneInfo = pMesh->m_pL3DBone->m_BoneInfo[nIndex];
    auto& bone = m_vecNormalBone.emplace_back(FLEXIBLE_BONE{ nIndex, boneInfo.sBoneName });

    XMStoreFloat4x4(&bone.InitPose, XMMatrixInverse(nullptr, pMesh->m_pL3DBone->m_BoneInvPxPose[nIndex]));
    pDriverBone->vecBoneChain.push_back(m_vecNormalBone.size() - 1);

    for (auto childIndex : boneInfo.ChildIndies)
        _AppendBoneOfChainFromMesh(pMesh, pDriverBone, childIndex);
}
