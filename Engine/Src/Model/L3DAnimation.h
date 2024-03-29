#pragma once

#include "L3DInterface.h"

#include "L3DBone.h"
#include <vector>
#include <string>

#include "IAnimation.h"

struct ANIMATION_UPDATE_INFO
{
    DWORD dwFrame   = 0;
    DWORD dwFrameTo = 0;
    float fWeight   = 0.f;
    struct _BoneAni
    {
        std::vector<XMMATRIX>* pBoneMatrix = nullptr; // �任����
        unsigned int nBoneCount = 0;
        const std::vector<BONEINFO>* pBoneInfo = nullptr; // ����������Ϣ
        unsigned int nFirstBaseBoneIndex = 0;
    } BoneAni;
};

class L3DAnimation
{
public:
    bool LoadFromFile(const char* szAnimation);
    void UpdateAnimation(ANIMATION_UPDATE_INFO* pUpdateAniInfo);

    void GetCurFrame(DWORD& dwFrame, DWORD& dwFrameTo, float& fWeight);

    void Start(AnimationPlayType nPlayType);
    void FrameMove();

    static void InterpolateRTSKeyFrame(RTS* pResult, const RTS& rtsA, const RTS& rtsB, float fWeight);

    void UpdateBone(ANIMATION_UPDATE_INFO* pUpdateAniInfo);

private:
    DWORD m_dwAniType    = 0;   // m_dwAniType
    DWORD m_dwNumBone    = 0;   // m_dwNumBone
    DWORD m_dwNumFrames  = 0;   // m_dwNumFrames
    float m_fFrameLength = 0.f; // m_fFrameLength
    DWORD m_nAnimationLen = 0;  // m_nAnimationLen

    AnimationPlayType   m_nPlayType = AnimationPlayType::Circle;
    unsigned int        m_nPlayTime = 0;
    unsigned int        m_nLastTime = 0;
    unsigned int        m_uCurrentFrame = 0;

    L3D::path m_Path;
    std::vector<std::string> m_BoneNames;
    std::vector<int> m_Flags;
    std::vector<std::vector<RTS>> m_BoneRTS;

    void _GetBoneMatrix(std::vector<XMMATRIX>& pResult, DWORD dwFrame, DWORD dwFrameTo, float fWeight);
    void _UpdateRTSRealTime(ANIMATION_UPDATE_INFO* pAnimationInfo);

    void _UpdateToObj(std::vector<XMMATRIX>& pBoneMatrix, const std::vector<BONEINFO>& BoneInfo, unsigned int uIndex, const XMMATRIX& mBase);
};

class L3DAnimationController
{
public:
    void UpdateAnimation();
    void SetBoneAniInfo(unsigned uBoneCount, const std::vector<BONEINFO>* pBoneInfo, unsigned int nFirsetBaseBoneIndex);
    void StartAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority);

    void FrameMove();

    ANIMATION_UPDATE_INFO m_UpdateAniInfo;
private:
    DWORD m_nPriority = 0;
    std::unique_ptr<L3DAnimation> m_pAnimation[ANICTL_COUNT] = { nullptr };

    std::vector<XMMATRIX> m_BoneMatrix;
};