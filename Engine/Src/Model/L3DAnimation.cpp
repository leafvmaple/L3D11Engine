#include "stdafx.h"
#include "L3DAnimation.h"

#include "Component/L3DTimer.h"
#include "Utility/L3DMaths.h"

#include "IAnimation.h"

HRESULT L3DAnimation::LoadFromFile(const char* szAnimation)
{
    ANIMATION_DESC desc { szAnimation };
    ANIMATION_SOURCE* pSource = nullptr;

    LoadAnimation(&desc, pSource);

    m_dwNumBone = pSource->nBonesCount;
    m_dwNumFrames = pSource->nFrameCount;
    m_fFrameLength = pSource->fFrameLength;
    m_nAnimationLen = pSource->nAnimationLength;

    m_BoneNames.resize(m_dwNumBone);
    for (unsigned int i = 0; i < m_dwNumBone; i++)
        m_BoneNames[i] = pSource->pBoneNames[i];

    m_BoneRTS.resize(m_dwNumBone);
    for (unsigned int i = 0; i < m_dwNumBone; i++)
    {
        m_BoneRTS[i].resize(m_dwNumFrames);
        for (unsigned int j = 0; j < m_dwNumFrames; j++)
            m_BoneRTS[i][j] = std::move(pSource->pBoneRTS[i][j]);
    }

    m_szName = szAnimation;

    return S_OK;
}

HRESULT L3DAnimation::UpdateAnimation(ANIMATION_UPDATE_INFO* pUpdateAniInfo)
{
    return _UpdateRTSRealTime(pUpdateAniInfo);
}

HRESULT L3DAnimation::GetCurFrame(DWORD dwAniPlayLen, AnimationPlayType dwPlayType, DWORD& dwFrame, DWORD& dwFrameTo, float& fWeight)
{
    DWORD dwSpanTime = dwAniPlayLen % m_nAnimationLen;
    DWORD dwRepeatTimes = dwAniPlayLen / m_nAnimationLen;

    dwFrame = (DWORD)(dwSpanTime / m_fFrameLength);
    fWeight = (dwSpanTime - dwFrame * m_fFrameLength) / m_fFrameLength;

    switch (dwPlayType)
    {
    case AnimationPlayType::Circle:
        dwFrameTo = (dwFrame + 1) % m_dwNumFrames;
        break;
    case AnimationPlayType::Once:
        if (dwRepeatTimes >= 1)
        {
            dwFrame = m_dwNumFrames - 1;
            dwFrameTo = dwFrame;
            fWeight = 1.0f;
        }
        else
        {
            dwFrameTo = dwFrame + 1;
        }
        break;
    }

    return S_OK;
}

void L3DAnimation::InterpolateRTSKeyFrame(RTS* pResult, const RTS& rtsA, const RTS& rtsB, float fWeight)
{
    L3D::XMFloat4Slerp(&pResult->SRotation, &rtsA.SRotation, &rtsB.SRotation, fWeight);
    L3D::XMFloat4Slerp(&pResult->Rotation, &rtsA.Rotation, &rtsB.Rotation, fWeight);
    L3D::XMFloat3Slerp(&pResult->Scale, &rtsA.Scale, &rtsB.Scale, fWeight);
    L3D::XMFloat3Slerp(&pResult->Translation, &rtsA.Translation, &rtsB.Translation, fWeight);

    pResult->Sign = rtsA.Sign;
}

void L3DAnimation::UpdateBone(ANIMATION_UPDATE_INFO* pUpdateAniInfo)
{
    XMMATRIX* pBoneMatrix = pUpdateAniInfo->BoneAni.pBoneMatrix->data();
    const BONEINFO* pBoneInfo = pUpdateAniInfo->BoneAni.pBoneInfo->data();
    unsigned int uFirstBaseBoneIndex = pUpdateAniInfo->BoneAni.nFirsetBaseBoneIndex;

    for (unsigned nChild : pBoneInfo[uFirstBaseBoneIndex].ChildIndies)
        _UpdateToObj(pBoneMatrix, pBoneInfo, nChild, pBoneMatrix[uFirstBaseBoneIndex]);
}

HRESULT L3DAnimation::_GetBoneMatrix(DWORD dwFrame, DWORD dwFrameTo, float fWeight, XMMATRIX* pResult)
{
    RTS rts;
    for (DWORD i = 0; i < m_dwNumBone; i++)
    {
        InterpolateRTSKeyFrame(&rts, m_BoneRTS[i][dwFrame], m_BoneRTS[i][dwFrameTo], fWeight);
        L3D::RTS2Matrix(pResult[i], rts);
    }

    return S_OK;
}

HRESULT L3DAnimation::_UpdateRTSRealTime(ANIMATION_UPDATE_INFO* pAnimationInfo)
{
    DWORD dwFrame = 0;
    DWORD dwFrameTo = 0;
    float fWeight = 0.f;

    dwFrame = pAnimationInfo->dwFrame;
    dwFrameTo = pAnimationInfo->dwFrameTo;
    fWeight = pAnimationInfo->fWeight;

    _GetBoneMatrix(dwFrame, dwFrameTo, fWeight, pAnimationInfo->BoneAni.pBoneMatrix->data());

    return S_OK;
}

void L3DAnimation::_UpdateToObj(XMMATRIX* pBoneMatrix, const BONEINFO* pBoneInfo, unsigned uIndex, const XMMATRIX& mBase)
{
    const BONEINFO& Bone = pBoneInfo[uIndex];

    pBoneMatrix[uIndex] = XMMatrixMultiply(pBoneMatrix[uIndex], mBase);

    for (unsigned nChild : Bone.ChildIndies)
        _UpdateToObj(pBoneMatrix, pBoneInfo, nChild, pBoneMatrix[uIndex]);
}

HRESULT L3DAnmationController::UpdateAnimation()
{
    m_pAnimation[m_nPriority]->UpdateAnimation(&m_UpdateAniInfo);
    m_pAnimation[m_nPriority]->UpdateBone(&m_UpdateAniInfo);

    return S_OK;
}

void L3DAnmationController::SetBoneAniInfo(unsigned uBoneCount, const std::vector<BONEINFO>* pBoneInfo, unsigned int nFirsetBaseBoneIndex)
{
    m_BoneMatrix.resize(uBoneCount);
    for (auto& bone : m_BoneMatrix)
        bone = XMMatrixIdentity();

    m_UpdateAniInfo.BoneAni.nBoneCount = uBoneCount;
    m_UpdateAniInfo.BoneAni.pBoneInfo = pBoneInfo;
    m_UpdateAniInfo.BoneAni.pBoneMatrix = &m_BoneMatrix;
    m_UpdateAniInfo.BoneAni.nFirsetBaseBoneIndex = nFirsetBaseBoneIndex;
}

HRESULT L3DAnmationController::StartAnimation(L3DAnimation* pAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    DWORD dwFrame = 0;
    DWORD dwFrameTo = 0;
    float fWeight = 0.f;

    if (m_nPriority < nPriority)
        m_nPriority = nPriority;

    SAFE_DELETE(m_pAnimation[nPriority]);

    m_pAnimation[nPriority] = pAnimation;
    m_nPlayType[nPriority]  = nPlayType;
    m_nLastTime[nPriority]  = g_Timer.GetNowTime();
    m_nPlayTime[nPriority]  = 0;

    pAnimation->GetCurFrame(0, nPlayType, dwFrame, dwFrameTo, fWeight);
    m_uCurrentFrame[nPriority] = dwFrame;

    return S_OK;
}

void L3DAnmationController::FrameMove()
{
    DWORD nFrame = 0;
    DWORD nFrameTo = 0;
    float fWeight = 0;

    unsigned int nNowTime = g_Timer.GetNowTime();

    for (int nPriority = 0; nPriority < ANICTL_COUNT; nPriority++)
    {
        if (!m_pAnimation[nPriority])
            continue;

        DWORD nTime = (DWORD)(nNowTime - m_nLastTime[nPriority]);

        m_nPlayTime[nPriority] += nTime;
        m_nLastTime[nPriority] = nNowTime;

        m_pAnimation[nPriority]->GetCurFrame(m_nPlayTime[nPriority], m_nPlayType[nPriority], nFrame, nFrameTo, fWeight);
        m_uCurrentFrame[nPriority] = nFrame;

        if (m_nPriority == nPriority)
        {
            m_UpdateAniInfo.dwFrame = nFrame;
            m_UpdateAniInfo.dwFrameTo = nFrameTo;
            m_UpdateAniInfo.fWeight = fWeight;
        }
    }
}
