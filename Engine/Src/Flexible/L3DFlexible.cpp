#include "stdafx.h"
#include "L3DFlexible.h"

#include "Model/L3DMesh.h"
#include "Model/L3DBone.h"

// 读取柔体骨骼
void L3DFlexible::Init(const L3DMesh* const pMesh)
{
    for (int nBoneIndex = 0; nBoneIndex < pMesh->m_dwBoneCount; nBoneIndex++) 
    {
        const auto& boneInfo = pMesh->m_pL3DBone->m_BoneInfo[nBoneIndex];
        if (boneInfo.sBoneName.starts_with("fbr"))
        {
            int nParentIndex = boneInfo.uParentIndex;
            const auto& parentInfo = pMesh->m_pL3DBone->m_BoneInfo[nParentIndex];

            auto it = m_vecDriverBone.try_emplace(nParentIndex, FLEXIBLE_BONE{ nParentIndex }).first;
            XMStoreFloat4x4(&it->second.InitPose, pMesh->m_BoneMatrix[nParentIndex]);

            _AppendBoneOfChainFromMesh(pMesh, &it->second, nBoneIndex);
        }
    }
}

// 更新柔体骨骼绝对矩阵
void L3DFlexible::Update(std::vector<XMMATRIX>& BoneMatrix, const XMFLOAT4X4& mWorld)
{
    XMMATRIX xmWorld;
    XMMATRIX xmWorldInvert;

    xmWorld = XMLoadFloat4x4(&mWorld);
    xmWorldInvert = XMMatrixInverse(nullptr, xmWorld);

    // TODO
    // 将柔体父骨骼的绝对变换矩阵转为世界矩阵，方便在世界中进行物理模拟
    for (auto& [nBoneIndex, driveBone] : m_vecDriverBone)
         XMStoreFloat4x4(&driveBone.WorldPose, XMMatrixMultiply(BoneMatrix[nBoneIndex], xmWorld));

    // if (m_FlexibleBodyUpdateState == KG3D_FLEX_UPDATE_FROM_ANI)
    _UpdateFromAni();

    // 将柔体骨骼世界矩阵转回绝对矩阵
    for (auto& bone : m_vecNormalBone)
        BoneMatrix[bone.nIndex] = XMMatrixMultiply(XMLoadFloat4x4(&bone.WorldPose), xmWorldInvert);
}

void L3DFlexible::_UpdateFromAni()
{
    XMMATRIX xmInvert;
    XMMATRIX xmMovement;

    for (auto& [key, driveBone] : m_vecDriverBone)
    {
        xmInvert = XMMatrixInverse(nullptr, XMLoadFloat4x4(&driveBone.InitPose));
        xmMovement = XMMatrixMultiply(xmInvert, XMLoadFloat4x4(&driveBone.WorldPose));

        for (auto child : driveBone.vecBoneChain)
        {
            FLEXIBLE_BONE& normalBone = m_vecNormalBone[child];
            XMStoreFloat4x4(&normalBone.WorldPose, XMLoadFloat4x4(&normalBone.InitPose) * xmMovement);
        }
    }

    m_FlexibleBodyUpdateState = KG3D_FLEX_UPDATE_FROM_PHYSX;
}

void L3DFlexible::_AppendBoneOfChainFromMesh(const L3DMesh* const pMesh, FLEXIBLE_BONE* pDriverBone, int nIndex)
{
    const auto& boneInfo = pMesh->m_pL3DBone->m_BoneInfo[nIndex];
    auto& bone = m_vecNormalBone.emplace_back(FLEXIBLE_BONE{ nIndex, boneInfo.sBoneName });

    XMStoreFloat4x4(&bone.InitPose, XMMatrixInverse(nullptr, pMesh->m_pL3DBone->m_BoneInvPxPose[nIndex]));
    pDriverBone->vecBoneChain.push_back(m_vecNormalBone.size() - 1);

    for (auto childIndex : boneInfo.ChildIndies)
        _AppendBoneOfChainFromMesh(pMesh, pDriverBone, childIndex);
}
