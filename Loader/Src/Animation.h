#pragma once
#include "IAnimation.h"

class LBinaryReader;

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

struct _BONE_ANI_V2
{
    DWORD dwNumBones;
    DWORD dwRealAniBones;
    DWORD dwNumFrames;
    float fFrameLength;
    BOOL  bHasBoneName;
    DWORD dwBoneMapHash;
};
#pragma pack(pop)

struct _RTSV2
{
    BYTE nFlag;
    XMFLOAT3 Translation;
    XMFLOAT3 Scale;
    XMFLOAT4 Rotation;
    XMFLOAT4 SRotation;
};

struct _CompressRotation
{
    SHORT wzyx[4] = { 0, 0, 0, SHRT_MAX };
};

struct _BONE_TRACK
{
    std::vector<_CompressRotation> Rotation;
    std::vector<XMFLOAT3> Translation;
    std::vector<float> fScale;
    std::vector<XMFLOAT3> Scale;
};

void _LoadAnimation(LBinaryReader* pReader, ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource);
void _LoadAnimationV2(LBinaryReader* pReader, ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource);