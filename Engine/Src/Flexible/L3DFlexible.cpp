#include "stdafx.h"
#include "L3DFlexible.h"

#include "Model/L3DMesh.h"
#include "Model/L3DBone.h"

// ��ȡ�������
void L3DFlexible::Init(const L3DMesh* const pMesh)
{
    for (int nBoneIndex = 0; nBoneIndex < pMesh->m_dwBoneCount; nBoneIndex++) 
    {
        const auto& boneInfo = pMesh->m_pL3DBone->m_BoneInfo[nBoneIndex];
        if (boneInfo.sBoneName.flexible_bone())
        {
            int nParentIndex = boneInfo.uParentIndex;
            const auto& parentInfo = pMesh->m_pL3DBone->m_BoneInfo[nParentIndex];

            auto it = m_vecDriverBone.try_emplace(nParentIndex, FLEXIBLE_BONE{ nParentIndex }).first;
            XMStoreFloat4x4(&it->second.InitPose, pMesh->m_BoneMatrix[nParentIndex]);

            _AppendBoneOfChainFromMesh(pMesh, &it->second, nBoneIndex);
        }
    }
}

// ��������������Ծ���
void L3DFlexible::Update(std::vector<XMMATRIX>& BoneMatrix, const XMFLOAT4X4& mWorld)
{
    XMMATRIX xmWorld;
    XMMATRIX xmWorldInvert;

    xmWorld = XMLoadFloat4x4(&mWorld);
    xmWorldInvert = XMMatrixInverse(nullptr, xmWorld);

    // TODO
    // �����常�����ľ��Ա任����תΪ������󣬷����������н�������ģ��
    for (auto& [nBoneIndex, driveBone] : m_vecDriverBone)
         XMStoreFloat4x4(&driveBone.WorldPose, XMMatrixMultiply(BoneMatrix[nBoneIndex], xmWorld));

    // if (m_FlexibleBodyUpdateState == KG3D_FLEX_UPDATE_FROM_ANI)
    _UpdateFromAni();

    // ����������������ת�ؾ��Ծ���
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
