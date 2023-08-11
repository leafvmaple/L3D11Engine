#pragma once

#include <memory>
#include <vector>
#include <string>

#include "L3DFormat.h"
#include "L3DInterface.h"

struct MESH_FILE_DATA;
struct MESH_FILE_BONE_INFO;
struct SKIN;

class LBinaryReader;

struct BONEINFO
{
	std::string sBoneName;
	unsigned int uParentIndex = (unsigned)-1;
	std::vector<unsigned int> ChildIndex;
};

struct L3D_BONE_INFO
{
	unsigned int uBoneCount = 0;

	std::vector<unsigned int> BaseBoneIndies;

	std::vector<L3D_BOUND_BOX> BoneBox;
	std::vector<XMMATRIX> BoneOffset; // ¹Ç÷ÀÆ«ÒÆÖµ
	std::vector<XMMATRIX> BoneInvPxPose;
	std::vector<BONEINFO> BoneInfo;
	unsigned int uSocketCount = 0;

	std::vector<std::string> OrderBoneName;
	std::vector<unsigned int> OrderIndex;
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
	unsigned int _FindBoneIndex(const char* szBoneName, const std::vector<std::string>& OrderBoneName, const std::vector<unsigned int>& OrderIndex);
};