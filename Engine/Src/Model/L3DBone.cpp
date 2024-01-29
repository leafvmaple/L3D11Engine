#include "stdafx.h"
#include "L3DBone.h"

#include <vector>

#include "L3DFormat.h"
#include "IO/LFileReader.h"

L3DBone::~L3DBone()
{
}

bool L3DBone::Create(const MESH_SOURCE* pSource)
{
    _CreateBoneInfo(pSource->pBones, pSource->nBonesCount);
    _CreateSocketInfo(pSource->pSockets, pSource->nSocketCount);

    return true;
}

void L3DBone::_CreateBoneInfo(const BONE_SOURCE* pSource, int nBoneCount)
{
    DWORD uBoneIndex = 0;
    std::vector<std::pair<std::string, unsigned int>> vecOrderBoneName;

    m_uBoneCount = nBoneCount;

    m_BoneBox.resize(nBoneCount);
    m_BoneOffset.resize(nBoneCount);
    m_BoneInvPxPose.resize(nBoneCount);
    m_BoneInfo.resize(nBoneCount);

    m_OrderBoneName.resize(nBoneCount);
    m_OrderIndex.resize(nBoneCount);

    for (unsigned int i = 0; i < nBoneCount; ++i)
    {
        m_BoneInfo[i].sBoneName = pSource[i].szName;
        vecOrderBoneName.emplace_back(make_pair(m_BoneInfo[i].sBoneName, i));
    }

    std::sort(vecOrderBoneName.begin(), vecOrderBoneName.end());
    for (size_t i = 0; i < vecOrderBoneName.size(); ++i)
    {
        m_OrderBoneName[i] = vecOrderBoneName[i].first;
        m_OrderIndex[i] = vecOrderBoneName[i].second;
    }

    for (unsigned int i = 0; i < nBoneCount; i++)
    {
        BONEINFO& Bone = m_BoneInfo[i];
        auto& BoneInFile = pSource[i];

        Bone.ChildIndies.resize(BoneInFile.nChildCount);
        for (DWORD j = 0; j < BoneInFile.nChildCount; ++j)
        {
            uBoneIndex = _FindBoneIndex(BoneInFile.pChildNames[j]);
            Bone.ChildIndies[j] = uBoneIndex;
            m_BoneInfo[uBoneIndex].uParentIndex = i;
        }

        m_BoneOffset[i] = XMLoadFloat4x4(&(BoneInFile.mOffset));
        m_BoneInvPxPose[i] = XMLoadFloat4x4(&(BoneInFile.mInvPxPose));
        m_BoneBox[i] = BoneInFile.BoundingBox;
    }

    for (unsigned i = 0; i < nBoneCount; ++i)
    {
        if (m_BoneInfo[i].uParentIndex == (unsigned)-1)
            m_BaseBoneIndies.push_back(i);
    }
}

void L3DBone::_CreateSocketInfo(const SOCKET_SOURCE* pSource, int nSocketCount)
{
    m_uSocketCount = nSocketCount;

    m_Socket.resize(nSocketCount);
    for (int i = 0; i < nSocketCount; i++)
    {
        SOCKETINFO& Socket = m_Socket[i];

        Socket.sSocketName = pSource[i].szName;
        Socket.uParentBoneIndex = _FindBoneIndex(pSource[i].szParentName);
        Socket.mOffset = pSource[i].mOffset;
    }
}

unsigned int L3DBone::_FindBoneIndex(const char* szBoneName)
{
    L3D::lower_string boneName(szBoneName);
    auto it = std::lower_bound(m_OrderBoneName.begin(), m_OrderBoneName.end(), boneName);
    if (*it == boneName)
        return m_OrderIndex[it - m_OrderBoneName.begin()];
    return -1;
}
