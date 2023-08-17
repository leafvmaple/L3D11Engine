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

struct L3D_BONE_INFO
{
	unsigned int uBoneCount = 0;

	std::vector<unsigned int> BaseBoneIndies;

	std::vector<L3D_BOUND_BOX> BoneBox;
	std::vector<XMMATRIX> BoneOffset; // ¹Ç÷ÀÆ«ÒÆÄæ¾ØÕó
	std::vector<XMMATRIX> BoneInvPxPose;
	std::vector<BONEINFO> BoneInfo;
	unsigned int uSocketCount = 0;

	std::vector<std::string> OrderBoneName;
	std::vector<unsigned int> OrderIndex;

	std::vector<SOCKETINFO> Socket;
};

class L3DBone
{
public:
	~L3DBone();

	HRESULT BindData(const MESH_FILE_BONE_INFO& BoneInfo);

	static HRESULT Load(MESH_FILE_BONE_INFO& BoneInfo, LBinaryReader& Reader, BOOL bHasPxPose, BOOL bHasBoundBox);
	static HRESULT FillSkinData(SKIN*& pSkin, MESH_FILE_BONE_INFO& BoneInfo, DWORD nVertexCount);

	L3D_BONE_INFO* m_pBoneInfo = nullptr;

private:
	void _CreateBoneInfo(const MESH_FILE_BONE_INFO& BoneInfo);
	void _CreateSocketInfo(const MESH_FILE_BONE_INFO& BoneInfo);
	unsigned int _FindBoneIndex(const char* szBoneName, const L3D_BONE_INFO* pBoneInfo);
};