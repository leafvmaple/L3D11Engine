#pragma once
#include "L3DInterface.h"
#include <vector>
#include <unordered_map>
#include <optional>

#include "L3DBone.h"

class L3DMesh;

class L3DSkeleton
{
public:
    HRESULT Create(ID3D11Device* piDevice, const char* szFileName);
    void BindMesh(const L3DMesh* p3DMesh);

    int FindBone(std::string szBoneName);

    unsigned int m_nNumBones = 0;
    std::vector<BONEINFO> m_BoneInfo;

    unsigned int m_uBaseBoneCount = 0;
    unsigned int m_uFirsetBaseBoneIndex = 0;

    std::string m_sName;

private:
    void _LineToBoneInfo(const char szLineBuffer[], BONEINFO& BoneInfo, std::vector<std::string>& childBone);
    void _ConstructSkeleton(unsigned uIndex, const std::vector<std::string>& childBone);
};

class L3DSkeletonManager
{
public:
    void PushNewData(std::string sKeletonName, std::string sMeshName, std::vector<int>& skeletonIndies);
    int* GetData(std::string sKeletonName, std::string sMeshName);
private:
    struct StringHash
    {
        size_t operator()(const std::pair<std::string, std::string>& p) const
        {
            std::size_t h1 = std::hash<std::string>{}(p.first);
            std::size_t h2 = std::hash<std::string>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
    typedef std::unordered_map<std::pair<std::string, std::string>, std::vector<int>, StringHash> HASH_TABLE;

    HASH_TABLE m_HashTable;
};

extern L3DSkeletonManager g_SkeletonBoneManager;