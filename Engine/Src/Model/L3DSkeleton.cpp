#include "stdafx.h"
#include <windows.h>
#include <cstdio>

#include "L3DSkeleton.h"
#include "L3DMesh.h"
#include "L3DBone.h"

L3DSkeletonManager g_SkeletonBoneManager;

HRESULT L3DSkeleton::Create(ID3D11Device* piDevice, const char* szFileName)
{
    char szLine[MAX_PATH];
    std::vector<std::vector<std::string>> childBoneNames;

    FILE* f = fopen(szFileName, "r");

    assert(fscanf(f, "%d\n", &m_nNumBones));
    m_BoneInfo.resize(m_nNumBones);
    childBoneNames.resize(m_nNumBones);

    for (int i = 0; i < m_nNumBones; i++)
    {
        fgets(szLine, MAX_PATH, f);
        szLine[strlen(szLine) - 1] = '\0';
        _LineToBoneInfo(szLine, m_BoneInfo[i], childBoneNames[i]);

        m_BoneInfo[i].uParentIndex = i;
    }

    for (int i = 0; i < m_nNumBones; i++)
        _ConstructSkeleton(i, childBoneNames[i]);

    assert(fscanf(f, "%d\n", &m_uBaseBoneCount));
    assert(fscanf(f, "%d\n", &m_uFirsetBaseBoneIndex));

    m_sName = szFileName;

    return S_OK;
}

void L3DSkeleton::BindMesh(const L3DMesh* p3DMesh)
{
    std::vector<int> skeletonBoneIndies;

    skeletonBoneIndies.resize(p3DMesh->m_dwBoneCount);
    for (int i = 0; i < p3DMesh->m_dwBoneCount; i++)
    {
        auto nBone = FindBone(p3DMesh->m_pL3DBone->m_pBoneInfo->BoneInfo[i].sBoneName);
        if (nBone)
            skeletonBoneIndies[i] = *nBone;
    }

    g_SkeletonBoneManager.PushNewData(m_sName, p3DMesh->m_sName, skeletonBoneIndies);
}

std::optional<int> L3DSkeleton::FindBone(std::string szBoneName)
{
    for (int i = 0; i < m_nNumBones; i++)
        if (m_BoneInfo[i].sBoneName == szBoneName)
            return i;
    return std::nullopt;
}

void L3DSkeleton::_LineToBoneInfo(const char szLineBuffer[], BONEINFO& BoneInfo, std::vector<std::string>& childBonee)
{
    char* szFind = nullptr;
    char* szLine = nullptr;
    bool bFirst = false;

    szLine = (char*)szLineBuffer;
    while (szFind = strstr(szLine, "  "))
    {
        *szFind = '\0';
        childBonee.push_back(szLine);
        szLine = szFind + 2;
    }
    childBonee.push_back(szLine);

    BoneInfo.sBoneName = childBonee[0];
}

void L3DSkeleton::_ConstructSkeleton(unsigned uIndex, const std::vector<std::string>& childBone)
{
    BONEINFO& Info = m_BoneInfo[uIndex];

    for (int i = 1; i < childBone.size(); i++)
    {
        auto nBone = FindBone(childBone[i]);
        if (nBone)
        {
            Info.ChildIndex.push_back(*nBone);
            m_BoneInfo[*nBone].uParentIndex = uIndex;
        }
    }
}

void L3DSkeletonManager::PushNewData(std::string sKeletonName, std::string sMeshName, std::vector<int>& skeletonIndies)
{
    m_HashTable.insert(std::make_pair(std::make_pair(sKeletonName, sMeshName), skeletonIndies));
}

int* L3DSkeletonManager::GetData(std::string sKeletonName, std::string sMeshName)
{
    auto value = m_HashTable.find(std::make_pair(sKeletonName, sMeshName));
    if (value != m_HashTable.end())
        return value->second.data();

    return nullptr;
}