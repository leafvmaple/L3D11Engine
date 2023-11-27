#pragma once

#include "LInterface.h"
#include "LAssert.h"

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

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
	const char* szFileName = nullptr;
};

struct RTS
{
    XMFLOAT3 Translation;
    XMFLOAT3 Scale;
    XMFLOAT4 Rotation;
    float Sign;
    XMFLOAT4 SRotation;
};

struct ANIMATION_SOURCE : LUnknown
{
	int nAnimationType = 0;

	int nBonesCount = 0;
	int nFrameCount = 0;
	float fFrameLength = 0;
	int nAnimationLength = 0;

	char (*pBoneNames)[ANI_STRING_SIZE] = nullptr;
	RTS** pBoneRTS = nullptr;

	~ANIMATION_SOURCE()
	{
		SAFE_DELETE_ARRAY(pBoneNames);
		if (nBonesCount > 0)
        {
            for (int i = 0; i < nBonesCount; i++)
                SAFE_DELETE_ARRAY(pBoneRTS[i]);
            SAFE_DELETE_ARRAY(pBoneRTS);
        }
	}
};

L3DENGINE_API void LoadAnimation(ANIMATION_DESC*pDesc, ANIMATION_SOURCE*& pSource);