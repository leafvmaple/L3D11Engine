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
	static const int MAX_NUM_CHILD_BONE = 60;

	std::string sBoneName;
	unsigned int uParentIndex = (unsigned)-1;
	unsigned int uNumChild = 0;
	unsigned int ChildIndex[MAX_NUM_CHILD_BONE] = {};
};

struct L3D_BONE_INFO
{
	unsigned int uBoneCount = 0;

	std::vector<unsigned int> BaseBoneIndex;

	std::vector<L3D_BOUND_BOX> BoneBox;
	std::vector<XMMATRIX> BoneOffset; // invBindPose
	std::vector<XMMATRIX> BoneInvPxPose;
	std::vector<BONEINFO> BoneInfo;
	unsigned int uSocketCount = 0;
	// SOCKETINFO* pSocket;

	std::vector<std::string> OrderBoneName;
	std::vector<unsigned int> OrderIndex;
};

class L3DBone
{
public:
	~L3DBone();

	HRESULT BindData(const MESH_FILE_BONE_INFO& BoneInfo);
	const L3D_BONE_INFO* GetBoneInfo() { return m_pBoneInfo; };

	static HRESULT Load(MESH_FILE_BONE_INFO& BoneInfo, LBinaryReader& Reader, BOOL bHasPxPose, BOOL bHasBoundBox);
	static HRESULT FillSkinData(SKIN*& pSkin, MESH_FILE_BONE_INFO& BoneInfo, DWORD nVertexCount);

private:
	unsigned int _FindBoneIndex(const char* szBoneName,
		const std::vector<std::string>& OrderBoneName,
		const std::vector<unsigned int>& OrderIndex);

	L3D_BONE_INFO* m_pBoneInfo = nullptr;
};