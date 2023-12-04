#pragma once
#include <vector>
#include <unordered_map>

#include "L3DInterface.h"
#include "Utility/L3DString.h"

class L3DMesh;

struct FLEXIBLE_BONE
{
    int nIndex = 0; // ������BoneMatrix�е�����
    L3D::lstring pcszName;
    XMFLOAT4X4 InitPose;
    XMFLOAT4X4 WorldPose;

    std::vector<int> vecBoneChain;
};


class L3DFlexible
{
public:
    void InitFromMesh(L3DMesh* pMesh);

    void Update(std::vector<XMMATRIX>& BoneMatrix, const XMFLOAT4X4& mWorld);
    void _UpdateFromAni();

private:
    enum KG3D_FLEX_UPDAT_STATE
    {
        KG3D_FLEX_UPDATE_FROM_PHYSX,
        KG3D_FLEX_UPDATE_FROM_ANI,
    };

    void _AppendBoneOfChainFromMesh(L3DMesh* pMesh, FLEXIBLE_BONE* pDriverBone, int nIndex);

    std::unordered_map<int, FLEXIBLE_BONE> m_vecDriverBone; // ��������������Ϣ����
    std::vector<FLEXIBLE_BONE> m_vecNormalBone; // ���������Ϣ����

    KG3D_FLEX_UPDAT_STATE m_FlexibleBodyUpdateState = KG3D_FLEX_UPDATE_FROM_ANI;
};