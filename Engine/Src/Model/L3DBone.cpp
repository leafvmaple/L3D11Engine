#include "stdafx.h"
#include "L3DBone.h"

#include <vector>

#include "L3DFormat.h"
#include "IO/LFileReader.h"

L3DBone::~L3DBone()
{
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

    Reader.Copy(&BoneInfo.dwSocketCount);
    if (BoneInfo.dwSocketCount > 0)
    {
        BoneInfo.pSocket = new MESH_FILE_BONE_INFO::SOCKET[BoneInfo.dwSocketCount];

        Reader.Copy(BoneInfo.pSocket, BoneInfo.dwSocketCount);
        std::sort(BoneInfo.pSocket, BoneInfo.pSocket + BoneInfo.dwSocketCount, [](MESH_FILE_BONE_INFO::SOCKET& left, MESH_FILE_BONE_INFO::SOCKET& right) {
            if (int nCmp = _stricmp(left.szName, right.szName))
                return nCmp < 0;
            return &left < &right;
            });
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

    _CreateBoneInfo(BoneInfo);
    _CreateSocketInfo(BoneInfo);

    return S_OK;
}

void L3DBone::_CreateBoneInfo(const MESH_FILE_BONE_INFO& BoneInfo)
{
    DWORD uBoneIndex = 0;
    DWORD uBoneCount = 0;
    std::vector<std::pair<std::string, unsigned int>> vecOrderBoneName;

    uBoneCount = BoneInfo.dwBoneCount;

    m_uBoneCount = uBoneCount;

    m_BoneBox.resize(uBoneCount);
    m_BoneOffset.resize(uBoneCount);
    m_BoneInvPxPose.resize(uBoneCount);
    m_BoneInfo.resize(uBoneCount);

    m_OrderBoneName.resize(uBoneCount);
    m_OrderIndex.resize(uBoneCount);

    for (unsigned int i = 0; i < uBoneCount; ++i)
    {
        m_BoneInfo[i].sBoneName = BoneInfo.pBone[i].szName;
        vecOrderBoneName.emplace_back(make_pair(m_BoneInfo[i].sBoneName, i));
    }

    std::sort(vecOrderBoneName.begin(), vecOrderBoneName.end());
    for (size_t i = 0; i < vecOrderBoneName.size(); ++i)
    {
        m_OrderBoneName[i] = vecOrderBoneName[i].first;
        m_OrderIndex[i] = vecOrderBoneName[i].second;
    }

    for (unsigned int i = 0; i < uBoneCount; i++)
    {
        BONEINFO& Bone = m_BoneInfo[i];
        MESH_FILE_BONE_INFO::BONE& BoneInFile = BoneInfo.pBone[i];

        Bone.ChildIndies.resize(BoneInFile.uChildCount);
        for (DWORD j = 0; j < BoneInFile.uChildCount; ++j)
        {
            uBoneIndex = _FindBoneIndex(BoneInFile.pChildNames[j]);
            Bone.ChildIndies[j] = uBoneIndex;
            m_BoneInfo[uBoneIndex].uParentIndex = i;
        }

        m_BoneOffset[i] = XMLoadFloat4x4(&(BoneInFile.mOffset));
        m_BoneInvPxPose[i] = XMLoadFloat4x4(&(BoneInFile.mInvPxPose));
        m_BoneBox[i] = BoneInFile.BoundingBox;
    }

    for (unsigned i = 0; i < uBoneCount; ++i)
    {
        if (m_BoneInfo[i].uParentIndex == (unsigned)-1)
            m_BaseBoneIndies.push_back(i);
    }
}

void L3DBone::_CreateSocketInfo(const MESH_FILE_BONE_INFO& BoneInfo)
{
    unsigned int nSocketCount = 0;

    nSocketCount = BoneInfo.dwSocketCount;

    m_uSocketCount = nSocketCount;

    m_Socket.resize(nSocketCount);
    for (int i = 0; i < nSocketCount; i++)
    {
        SOCKETINFO& Socket = m_Socket[i];

        Socket.sSocketName = BoneInfo.pSocket[i].szName;
        Socket.uParentBoneIndex = _FindBoneIndex(BoneInfo.pSocket[i].szParentName);
        Socket.mOffset = BoneInfo.pSocket[i].mOffset;
    }
}

unsigned int L3DBone::_FindBoneIndex(const char* szBoneName)
{
    L3D::lstring boneName(szBoneName);
    auto it = std::lower_bound(m_OrderBoneName.begin(), m_OrderBoneName.end(), boneName);
    if (*it == boneName)
        return m_OrderIndex[it - m_OrderBoneName.begin()];
    return -1;
}
