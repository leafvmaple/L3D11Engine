#include "stdafx.h"
#include "L3DAnimation.h"
#include "IO/LFileReader.h"

#define ANI_STRING_SIZE 30
const unsigned	ANI_FILE_MASK = 0x414E494D;

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

HRESULT L3DAnimation::LoadFromFile(const char* szAnimation)
{
    LBinaryReader Reader;

    _ANI_FILE_HEADER* pHead = nullptr;
    _BONE_ANI* pBoneAni = nullptr;

    Reader.Init(szAnimation);
    Reader.Convert(pHead);

    assert(pHead->dwType == ANIMATION_BONE_RTS);
    assert(pHead->dwMask == ANI_FILE_MASK);

    Reader.Convert(pBoneAni);
    m_dwNumBone = pBoneAni->dwNumBones;
    m_dwNumFrames = pBoneAni->dwNumFrames;
    m_fFrameLength = pBoneAni->fFrameLength;

    m_BoneNames.resize(m_dwNumBone);
    for (int i = 0; i < m_dwNumBone; i++)
    {
        char* pszBoneName = nullptr;
        Reader.Convert(pszBoneName, ANI_STRING_SIZE);
        m_BoneNames[i] = pszBoneName;
    }

    m_BoneRTS.resize(m_dwNumBone);
    for (int i = 0; i < m_dwNumBone; i++)
        Reader.Copy(&m_BoneRTS[i]);

    m_szName = szAnimation;

    return S_OK;
}
