#pragma once

#include "L3DInterface.h"

#include "L3DBone.h"
#include <vector>
#include <string>

struct ANIMATION_UPDATE_INFO
{
    DWORD dwFrame   = 0;
    DWORD dwFrameTo = 0;
    float fWeight   = 0.f;
    struct _BoneAni
    {
        std::vector<XMMATRIX>* pBoneMatrix = nullptr;
        unsigned int nBoneCount = 0;
        const std::vector<BONEINFO>* pBoneInfo = nullptr;
        unsigned int nFirsetBaseBoneIndex = 0;
    } BoneAni;
};

class L3DAnimation
{
public:
    HRESULT LoadFromFile(const char* szAnimation);
    HRESULT UpdateAnimation(ANIMATION_UPDATE_INFO* pUpdateAniInfo);
    HRESULT GetCurFrame(DWORD dwAniPlayLen, DWORD dwPlayType, DWORD& dwFrame, DWORD& dwFrameTo, float& fWeight);

    static void InterpolateRTSKeyFrame(RTS* pResult, const RTS& rtsA, const RTS& rtsB, float fWeight);
    static void RTS2Matrix(XMMATRIX& mResult, const RTS& rts);

    void UpdateBone(ANIMATION_UPDATE_INFO* pUpdateAniInfo);

private:
    DWORD m_dwAniType    = 0;
    DWORD m_dwNumBone    = 0;
    DWORD m_dwNumFrames  = 0;
    float m_fFrameLength = 0.f;
    DWORD m_nAnimationLen     = 0;

    std::string m_szName;
    std::vector<std::string> m_BoneNames;
    std::vector<std::vector<RTS>> m_BoneRTS;

    HRESULT _GetBoneMatrix(DWORD dwFrame, DWORD dwFrameTo, float fWeight, XMMATRIX* pResult);
    HRESULT _UpdateRTSRealTime(ANIMATION_UPDATE_INFO* pAnimationInfo);

    void _UpdateToObj(XMMATRIX* pBoneMatrix, const BONEINFO* pBoneInfo, unsigned uIndex, const XMMATRIX& mBase);
};

class L3DAnmationController
{
public:
    HRESULT UpdateAnimation();
    void SetBoneAniInfo(unsigned uBoneCount, const std::vector<BONEINFO>* pBoneInfo, unsigned int nFirsetBaseBoneIndex);
    HRESULT StartAnimation(L3DAnimation* pAnimation, ANIMATION_PLAY_TYPE nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority);

    void FrameMove();

    ANIMATION_UPDATE_INFO m_UpdateAniInfo;
private:
    DWORD m_nPriority = 0;
    L3DAnimation* m_pAnimation[ANICTL_COUNT] = { nullptr };


    ANIMATION_PLAY_TYPE m_nPlayType[ANICTL_COUNT] = { ENU_ANIMATIONPLAY_CIRCLE };
    unsigned int        m_nPlayTime[ANICTL_COUNT] = { 0 };
    unsigned int        m_nLastTime[ANICTL_COUNT] = { 0 };
    unsigned int    m_uCurrentFrame[ANICTL_COUNT] = { 0 };

    std::vector<XMMATRIX> m_BoneMatrix;
};