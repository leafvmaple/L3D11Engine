#pragma once

#include "LInterface.h"
#include "LAssert.h"

#define ANI_STRING_SIZE 30
const unsigned	ANI_FILE_MASK = 0x414E494D;

enum ANIMATION_TYPE
{
	ANIMATION_NONE = 0,
	ANIMATION_VERTICES = 1,
	ANIMATION_VERTICES_RELATIVE = 2,
	ANIMATION_BONE = 10,
	ANIMATION_BONE_RELATIVE = 11,
	ANIMATION_BONE_16 = 12,
	ANIMATION_BONE_RTS = 13,
	ANIMATION_BLENDSHAPE = 14,
	ANIMATION_VCIK = 15,	//视频捕捉动画关键点转FullBodyIK
	ANIMATION_COUNT,
	ANIMATION_FORCE_DWORD = 0xffffffff,
};

#pragma pack(push,1)
struct _ANI_FILE_HEADER
{
	DWORD dwMask;
	DWORD dwBlockLength;
	DWORD dwNumAnimations;
	DWORD dwType;
	char  strDesc[ANI_STRING_SIZE];
};

struct _BONE_ANI
{
	DWORD dwNumBones;
	DWORD dwNumFrames;
	float fFrameLength;
};
#pragma pack(pop)

struct ANIMATION_DESC
{
	const wchar_t* szFileName = nullptr;
};

struct ANIMATION_SOURCE : LUnknown
{
	int nAnimationType = 0;

	int nBonesCount = 0;
	int nFrameCount = 0;
	int fFrameLength = 0;
	int nAnimationLength = 0;

	char (*pBoneNames)[MAX_PATH] = nullptr;
};

L3DENGINE_API void LoadFromFile(ANIMATION_DESC *pDesc, ANIMATION_SOURCE*& pSource);