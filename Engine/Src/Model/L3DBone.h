#pragma once

#include <memory>
#include <vector>
#include <string>

#include "L3DFormat.h"
#include "L3DInterface.h"

#define MAX_NUM_CHILD_BONE 60

struct MESH_FILE_BONE_INFO;
struct SKIN;

struct BONEINFO
{
	std::string sBoneName;
	unsigned uParentIndex;
	unsigned uNumChild;
	unsigned ChildIndex[MAX_NUM_CHILD_BONE];
	BONEINFO()
	{
		uParentIndex = (unsigned)-1;
		uNumChild = 0;
	}
};

struct MESH_FILE_DATA;

struct L3D_BONE_INFO
{
	unsigned uBoneCount;

	std::vector<unsigned int> BaseBoneIndex;

	std::vector<L3D_BOUND_BOX> BoneBox;
	std::vector<XMMATRIX> BoneOffset; // invBindPose
	std::vector<XMMATRIX> BoneInvPxPose;
	std::vector<BONEINFO> BoneInfo;
	unsigned uSocketCount;
	// SOCKETINFO* pSocket;

	std::vector<std::string> OrderBoneName;
	std::vector<unsigned int> OrderIndex;

	unsigned    FindBoneIndex(const char* pcszBoneName);
	const char* FindBoneName(unsigned uBoneIndex);
	//static bool  IsFlexibleBone(const char *pcszBoneName);
};

class LBinaryReader;

class L3DBone
{
public:
	~L3DBone();

	HRESULT Create(MESH_FILE_DATA* pMeshFileData, LBinaryReader& Reader, BOOL bHasPxPose, BOOL bHasBoundBox);
	HRESULT BindData(MESH_FILE_BONE_INFO& BoneInfo);

	const L3D_BONE_INFO* GetBoneInfo() { return m_pBoneInfo; };

private:
	HRESULT _Load(MESH_FILE_BONE_INFO& BoneInfo, LBinaryReader& Reader, BOOL bHasPxPose, BOOL bHasBoundBox);
	HRESULT _FillSkinData(SKIN*& pSkin, MESH_FILE_BONE_INFO& BoneInfo, DWORD nVertexCount);

	unsigned int _FindBoneIndex(const char* szBoneName,
		const std::vector<std::string>& OrderBoneName,
		const std::vector<unsigned int>& OrderIndex);

	DWORD m_dwBoneCount = 0;
	L3D_BONE_INFO* m_pBoneInfo = nullptr;
};