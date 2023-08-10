#include "stdafx.h"
#include "L3DBone.h"

#include <vector>

#include "L3DFormat.h"
#include "IO/LFileReader.h"

L3DBone::~L3DBone()
{
    SAFE_DELETE(m_pBoneInfo);
}

HRESULT L3DBone::Load(MESH_FILE_BONE_INFO& BoneInfo, LBinaryReader& Reader, BOOL bHasPxPose, BOOL bHasBoundBox)
{
    Reader.Copy(&BoneInfo.dwBoneCount);

    BoneInfo.pBone = new MESH_FILE_BONE_INFO::BONE[BoneInfo.dwBoneCount];

    for (DWORD i = 0; i < BoneInfo.dwBoneCount; i++)
    {
        MESH_FILE_BONE_INFO::BONE& Bone = BoneInfo.pBone[i];

        Reader.Copy(Bone.szName, 30);
        Reader.Seek(30, SEEK::CURSOR);

        Reader.Convert(Bone.uChildCount);
        Bone.pChildNames = nullptr;
        if (Bone.uChildCount > 0)
        {
            Bone.pChildNames = new MESH_FILE_BONE_INFO::BONE::BONE_NAME[Bone.uChildCount];
            Reader.Convert(Bone.pChildNames, Bone.uChildCount);
        }

        Reader.Copy(&Bone.mOffset);
        Reader.Copy(&Bone.mOffset2Parent);

        if (bHasPxPose)
            Reader.Copy(&Bone.mInvPxPose);

        Reader.Copy(&Bone.uRefVertexCount);
        if (Bone.uRefVertexCount > 0)
        {
            Bone.pRefVertexIndex = new DWORD[Bone.uRefVertexCount];
            Bone.pRefVertexWeight = new float[Bone.uRefVertexCount];

            Reader.Copy(Bone.pRefVertexIndex, Bone.uRefVertexCount);
            Reader.Copy(Bone.pRefVertexWeight, Bone.uRefVertexCount);
        }

        if (bHasBoundBox)
        {
            Reader.Seek(sizeof(XMFLOAT4X4), SEEK::CURSOR);
            Reader.Copy(&Bone.BoundingBox);
            Reader.Seek(sizeof(BOOL), SEEK::CURSOR);
        }
    }

    return S_OK;
}

HRESULT L3DBone::FillSkinData(SKIN*& pSkin, MESH_FILE_BONE_INFO& BoneInfo, DWORD nVertexCount)
{
    pSkin = new SKIN[nVertexCount];
    memset(pSkin, 0xFF, sizeof(pSkin[0]) * nVertexCount);

    for (unsigned i = 0; i < BoneInfo.dwBoneCount; i++)
    {
        MESH_FILE_BONE_INFO::BONE& Bone = BoneInfo.pBone[i];

        for (unsigned j = 0; j < Bone.uRefVertexCount; j++)
        {
            unsigned uVertexIndex = Bone.pRefVertexIndex[j];
            float fWeight = Bone.pRefVertexWeight[j];

            assert(uVertexIndex < nVertexCount);

            float* pBoneWeights = pSkin[uVertexIndex].BoneWeights;
            BYTE* pBoneIndices = pSkin[uVertexIndex].BoneIndices;

            for (unsigned k = 0; k < MAX_BONE_INFLUNCE; k++)
            {
                if (pBoneIndices[k] == 0xFF)
                {
                    pBoneIndices[k] = (BYTE)i;
                    pBoneWeights[k] = fWeight;
                    break;
                }
            }
        }
    }

    return S_OK;
}


HRESULT L3DBone::BindData(const MESH_FILE_BONE_INFO& BoneInfo)
{
    HRESULT hResult = E_FAIL;
    DWORD uBoneCount = 0;
    DWORD uBoneIndex = 0;
    std::vector<std::pair<std::string, unsigned int>> vecOrderBoneName;

    uBoneCount = BoneInfo.dwBoneCount;

    m_pBoneInfo = new L3D_BONE_INFO;
    m_pBoneInfo->uBoneCount = uBoneCount;

    m_pBoneInfo->BoneBox.resize(uBoneCount);
    m_pBoneInfo->BoneOffset.resize(uBoneCount);
    m_pBoneInfo->BoneInvPxPose.resize(uBoneCount);
    m_pBoneInfo->BoneInfo.resize(uBoneCount);

    m_pBoneInfo->OrderBoneName.resize(uBoneCount);
    m_pBoneInfo->OrderIndex.resize(uBoneCount);

    for (unsigned int i = 0; i < uBoneCount; ++i)
    {
        m_pBoneInfo->BoneInfo[i].sBoneName = BoneInfo.pBone[i].szName;
        vecOrderBoneName.emplace_back(make_pair(m_pBoneInfo->BoneInfo[i].sBoneName, i));
    }

    std::sort(vecOrderBoneName.begin(), vecOrderBoneName.end());
    for (size_t i = 0; i < vecOrderBoneName.size(); ++i)
    {
        m_pBoneInfo->OrderBoneName[i] = vecOrderBoneName[i].first;
        m_pBoneInfo->OrderIndex[i] = vecOrderBoneName[i].second;
    }

    for (unsigned int i = 0; i < uBoneCount; i++)
    {
        BONEINFO& Bone = m_pBoneInfo->BoneInfo[i];
        MESH_FILE_BONE_INFO::BONE& BoneInFile = BoneInfo.pBone[i];

        Bone.ChildIndex.resize(BoneInFile.uChildCount);
        for (DWORD j = 0; j < BoneInFile.uChildCount; ++j)
        {
            uBoneIndex = _FindBoneIndex(BoneInFile.pChildNames[j], m_pBoneInfo->OrderBoneName, m_pBoneInfo->OrderIndex);
            Bone.ChildIndex[j] = uBoneIndex;
            m_pBoneInfo->BoneInfo[uBoneIndex].uParentIndex = i;
        }

        m_pBoneInfo->BoneOffset[i] = XMLoadFloat4x4(&(BoneInFile.mOffset));
        m_pBoneInfo->BoneInvPxPose[i] = XMLoadFloat4x4(&(BoneInFile.mInvPxPose));
        m_pBoneInfo->BoneBox[i] = BoneInFile.BoundingBox;
    }

    for (unsigned i = 0; i < uBoneCount; ++i)
    {
        if (m_pBoneInfo->BoneInfo[i].uParentIndex == (unsigned)-1)
            m_pBoneInfo->BaseBoneIndex.push_back(i);
    }

    return S_OK;
}

unsigned int L3DBone::_FindBoneIndex(const char* szBoneName,
    const std::vector<std::string>& OrderBoneName,
    const std::vector<unsigned int>& OrderIndex)
{
    auto it = std::lower_bound(OrderBoneName.begin(), OrderBoneName.end(), szBoneName);
    if (*it == szBoneName)
        return OrderIndex[it - OrderBoneName.begin()];
    return -1;
}
