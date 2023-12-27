#pragma once

#include "LInterface.h"
#include "LAssert.h"

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

#define ANI_STRING_SIZE 30
const unsigned	ANI_FILE_MASK = 0x414E494D;
const unsigned	ANI_FILE_MASK_VERVION2 = 0x324E494D;

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

enum BONE_FLAG
{
    BONE_FLAG_NONE = 0,
    BONE_FLAG_AFFLINE = 1 << 0,
    BONE_FLAG_NO_UPDATE = 1 << 1,
};

struct ANIMATION_DESC
{
    const char* szFileName = nullptr;
};

struct RTS
{
    XMFLOAT3 Translation = { 0.f, 0.f, 0.f };
    XMFLOAT3 Scale = { 1.f, 1.f, 1.f };
    XMFLOAT4 Rotation = { 1.f, 0.f, 0.f, 0.f };
    float Sign = 1.f;
    XMFLOAT4 SRotation = { 1.f, 0.f, 0.f, 0.f };
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

    int* pFlag = nullptr;

    virtual ~ANIMATION_SOURCE() {
        SAFE_DELETE_ARRAY(pBoneNames);
        SAFE_DELETE_ARRAY(pFlag);
        if (nBonesCount > 0)
        {
            for (int i = 0; i < nBonesCount; i++)
                SAFE_DELETE_ARRAY(pBoneRTS[i]);
            SAFE_DELETE_ARRAY(pBoneRTS);
        }
    }
};

L3DENGINE_API void LoadAnimation(ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource);