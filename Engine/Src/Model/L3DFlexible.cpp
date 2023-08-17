#include "stdafx.h"
#include "L3DFlexible.h"
#include "L3DMesh.h"
#include "L3DBone.h"

void L3DFlexible::InitFromMesh(L3DMesh* pMesh)
{
    for (int i = 0; i < pMesh->m_dwBoneCount; i++) 
    {
        const auto& boneInfo = pMesh->m_pL3DBone->m_pBoneInfo->BoneInfo[i];
        if (boneInfo.sBoneName.IsFlexibleBone())
        {
            int nParentIndex = pMesh->m_pL3DBone->m_pBoneInfo->BoneInfo[i].uParentIndex;
            const auto& parentInfo = pMesh->m_pL3DBone->m_pBoneInfo->BoneInfo[nParentIndex];
            FLEXIBLE_BONE* pDriverBone = nullptr;

            if (m_vecDriverBone.find(nParentIndex) == m_vecDriverBone.end())
                m_vecDriverBone[nParentIndex] = FLEXIBLE_BONE { nParentIndex, parentInfo.sBoneName };

            pDriverBone = &m_vecDriverBone[nParentIndex];

            XMStoreFloat4x4(&pDriverBone->InitPose, XMMatrixInverse(nullptr, pMesh->m_pL3DBone->m_pBoneInfo->BoneOffset[nParentIndex]));
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
    const auto& boneInfo = pMesh->m_pL3DBone->m_pBoneInfo->BoneInfo[nIndex];
    auto& bone = m_vecNormalBone.emplace_back(FLEXIBLE_BONE{ nIndex, boneInfo.sBoneName });

    XMStoreFloat4x4(&bone.InitPose, XMMatrixInverse(nullptr, pMesh->m_pL3DBone->m_pBoneInfo->BoneInvPxPose[nIndex]));
    pDriverBone->vecBoneChain.push_back(m_vecNormalBone.size() - 1);

    for (auto childIndex : pMesh->m_pL3DBone->m_pBoneInfo->BoneInfo[nIndex].ChildIndies)
        _AppendBoneOfChainFromMesh(pMesh, pDriverBone, childIndex);
}
