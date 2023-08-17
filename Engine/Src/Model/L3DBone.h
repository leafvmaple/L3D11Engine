#pragma once

#include <memory>
#include <vector>
#include <string>

#include "L3DFormat.h"
#include "L3DInterface.h"

#include "Utility/L3DString.h"

struct MESH_FILE_DATA;
struct MESH_FILE_BONE_INFO;
struct SKIN;

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

	HRESULT BindData(const MESH_FILE_BONE_INFO& BoneInfo);

	static HRESULT Load(MESH_FILE_BONE_INFO& BoneInfo, LBinaryReader& Reader, BOOL bHasPxPose, BOOL bHasBoundBox);
	static HRESULT FillSkinData(SKIN*& pSkin, MESH_FILE_BONE_INFO& BoneInfo, DWORD nVertexCount);

    unsigned int m_uBoneCount = 0;

    std::vector<unsigned int> m_BaseBoneIndies;

    std::vector<L3D_BOUND_BOX> m_BoneBox;
    std::vector<XMMATRIX> m_BoneOffset; // ¹Ç÷ÀÆ«ÒÆÄæ¾ØÕó
    std::vector<XMMATRIX> m_BoneInvPxPose;
    std::vector<BONEINFO> m_BoneInfo;
    unsigned int m_uSocketCount = 0;

    std::vector<std::string> m_OrderBoneName;
    std::vector<unsigned int> m_OrderIndex;

    std::vector<SOCKETINFO> m_Socket;

private:
	void _CreateBoneInfo(const MESH_FILE_BONE_INFO& BoneInfo);
	void _CreateSocketInfo(const MESH_FILE_BONE_INFO& BoneInfo);
	unsigned int _FindBoneIndex(const char* szBoneName);
};