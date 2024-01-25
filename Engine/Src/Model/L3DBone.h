#pragma once

#include <memory>
#include <vector>
#include <string>

#include "L3DFormat.h"
#include "L3DInterface.h"
#include "IMesh.h"

#include "Utility/L3DString.h"

class LBinaryReader;

struct BONEINFO
{
	L3D::lstring sBoneName;
	unsigned int uParentIndex = (unsigned)-1;
	std::vector<unsigned int> ChildIndies; // ChildIndex
};

struct SOCKETINFO
{
	L3D::lstring sSocketName;
    unsigned int uParentBoneIndex = (unsigned)-1;
	XMFLOAT4X4 mOffset;
};

class L3DBone
{
public:
	~L3DBone();

	bool Create(const MESH_SOURCE* pSource);

    unsigned int m_uBoneCount = 0;

    std::vector<unsigned int> m_BaseBoneIndies;

    std::vector<L3D_BOUND_BOX> m_BoneBox;
    std::vector<XMMATRIX> m_BoneOffset; // ³õÊ¼×´Ì¬¹Ç÷À¾ø¶Ô±ä»»¾ØÕóµÄÄæ
    std::vector<XMMATRIX> m_BoneInvPxPose;
    std::vector<BONEINFO> m_BoneInfo;
    unsigned int m_uSocketCount = 0;

    std::vector<std::string> m_OrderBoneName;
    std::vector<unsigned int> m_OrderIndex;

    std::vector<SOCKETINFO> m_Socket;

private:
	void _CreateBoneInfo(const BONE_SOURCE* pSource, int nBoneCount);
	void _CreateSocketInfo(const SOCKET_SOURCE* pSource, int nSocketCount);
	unsigned int _FindBoneIndex(const char* szBoneName);
};