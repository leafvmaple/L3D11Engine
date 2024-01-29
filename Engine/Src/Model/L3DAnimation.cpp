#include "stdafx.h"
#include "L3DAnimation.h"

#include "Component/L3DTimer.h"
#include "Utility/L3DMaths.h"

#include "IAnimation.h"

bool L3DAnimation::LoadFromFile(const char* szAnimation)
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
        m_BoneNames[i] = std::move(pSource->pBoneNames[i]);

    m_BoneRTS.resize(m_dwNumBone);
    for (unsigned int i = 0; i < m_dwNumBone; i++)
    {
        m_BoneRTS[i].resize(m_dwNumFrames);
        for (unsigned int j = 0; j < m_dwNumFrames; j++)
            m_BoneRTS[i][j] = std::move(pSource->pBoneRTS[i][j]);
    }

    m_Flags.resize(m_dwNumBone);
    if (pSource->pFlag)
        memcpy(m_Flags.data(), pSource->pFlag, sizeof(int) * m_dwNumBone);

    m_Path = szAnimation;

    SAFE_RELEASE(pSource);

    return true;
}

// 根据当前帧状态计算并骨骼相对旋转矩阵并更新到pBoneMatrix
void L3DAnimation::UpdateAnimation(ANIMATION_UPDATE_INFO* pUpdateAniInfo)
{
    _UpdateRTSRealTime(pUpdateAniInfo);
}

// 获取当前状态前后帧以及权重
void L3DAnimation::GetCurFrame(DWORD& dwFrame, DWORD& dwFrameTo, float& fWeight)
{
    DWORD dwSpanTime = m_nPlayTime % m_nAnimationLen;
    DWORD dwRepeatTimes = m_nPlayTime / m_nAnimationLen;

    dwFrame = m_uCurrentFrame;
    fWeight = (dwSpanTime - dwFrame * m_fFrameLength) / m_fFrameLength;

    switch (m_nPlayType)
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
}

void L3DAnimation::Start(AnimationPlayType nPlayType)
{
    m_nLastTime = g_Timer.GetNowTime();
    m_nPlayType = nPlayType;
    m_nPlayTime = 0;
    m_uCurrentFrame = 0;
}

// 更新当前帧状态
void L3DAnimation::FrameMove()
{
    unsigned int nNowTime = g_Timer.GetNowTime();

    m_nPlayTime += nNowTime - m_nLastTime;
    m_nLastTime = nNowTime;

    m_uCurrentFrame = (DWORD)((m_nPlayTime % m_nAnimationLen) / m_fFrameLength);
}

// RTS关键帧线性插值
void L3DAnimation::InterpolateRTSKeyFrame(RTS* pResult, const RTS& rtsA, const RTS& rtsB, float fWeight)
{
    L3D::XMFloat4Slerp(&pResult->SRotation, &rtsA.SRotation, &rtsB.SRotation, fWeight);
    L3D::XMFloat4Slerp(&pResult->Rotation, &rtsA.Rotation, &rtsB.Rotation, fWeight);
    L3D::XMFloat3Slerp(&pResult->Scale, &rtsA.Scale, &rtsB.Scale, fWeight);
    L3D::XMFloat3Slerp(&pResult->Translation, &rtsA.Translation, &rtsB.Translation, fWeight);

    pResult->Sign = rtsA.Sign;
}

// 递归将相对变换矩阵更新为绝对变换矩阵
void L3DAnimation::UpdateBone(ANIMATION_UPDATE_INFO* pUpdateAniInfo)
{
    auto& BoneMatrix = *pUpdateAniInfo->BoneAni.pBoneMatrix;
    const auto& BoneInfo = *pUpdateAniInfo->BoneAni.pBoneInfo;
    unsigned int uFirstBaseBoneIndex = pUpdateAniInfo->BoneAni.nFirstBaseBoneIndex;

    for (unsigned int nChild : BoneInfo[uFirstBaseBoneIndex].ChildIndies)
        _UpdateToObj(BoneMatrix, BoneInfo, nChild, BoneMatrix[uFirstBaseBoneIndex]);
}

// 根据帧状态进行骨骼相对旋转矩阵线性插值
void L3DAnimation::_GetBoneMatrix(std::vector<XMMATRIX>& result, DWORD dwFrame, DWORD dwFrameTo, float fWeight)
{
    RTS rts;

    for (DWORD i = 0; i < m_dwNumBone; i++)
    {
        InterpolateRTSKeyFrame(&rts, m_BoneRTS[i][dwFrame], m_BoneRTS[i][dwFrameTo], fWeight);
        L3D::RTS2Matrix(result[i], rts, m_Flags[i]);
    }
}

// 根据当前帧状态计算骨骼相对旋转矩阵并更新到pBoneMatrix
void L3DAnimation::_UpdateRTSRealTime(ANIMATION_UPDATE_INFO* pAnimationInfo)
{
    _GetBoneMatrix(*pAnimationInfo->BoneAni.pBoneMatrix, pAnimationInfo->dwFrame, pAnimationInfo->dwFrameTo, pAnimationInfo->fWeight);
}

// 计算uIndex号骨骼到根骨骼的绝对旋转矩阵
void L3DAnimation::_UpdateToObj(std::vector<XMMATRIX>& BoneMatrix, const std::vector<BONEINFO>& BoneInfo, unsigned int uIndex, const XMMATRIX& mBase)
{
    const BONEINFO& Bone = BoneInfo[uIndex];

    BoneMatrix[uIndex] = XMMatrixMultiply(BoneMatrix[uIndex], mBase);

    for (unsigned int nChild : Bone.ChildIndies)
        _UpdateToObj(BoneMatrix, BoneInfo, nChild, BoneMatrix[uIndex]);
}

// 根据当前帧状态计算骨骼绝对变换矩阵并更新到pBoneMatrix
void L3DAnimationController::UpdateAnimation()
{
    m_pAnimation[m_nPriority]->UpdateAnimation(&m_UpdateAniInfo);
    m_pAnimation[m_nPriority]->UpdateBone(&m_UpdateAniInfo);
}

void L3DAnimationController::SetBoneAniInfo(unsigned uBoneCount, const std::vector<BONEINFO>* pBoneInfo, unsigned int nFirsetBaseBoneIndex)
{
    m_BoneMatrix.assign(uBoneCount, XMMatrixIdentity());

    m_UpdateAniInfo.BoneAni.nBoneCount = uBoneCount;
    m_UpdateAniInfo.BoneAni.pBoneInfo = pBoneInfo;
    m_UpdateAniInfo.BoneAni.pBoneMatrix = &m_BoneMatrix;
    m_UpdateAniInfo.BoneAni.nFirstBaseBoneIndex = nFirsetBaseBoneIndex;
}

void L3DAnimationController::StartAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    if (m_nPriority < nPriority)
        m_nPriority = nPriority;

    assert(nPriority < _countof(m_pAnimation));

    m_pAnimation[nPriority] = std::make_unique<L3DAnimation>();

    m_pAnimation[nPriority]->LoadFromFile(szAnimation);
    m_pAnimation[nPriority]->Start(nPlayType);
}

void L3DAnimationController::FrameMove()
{
    for (int nPriority = 0; nPriority < ANICTL_COUNT; nPriority++)
    {
        if (!m_pAnimation[nPriority])
            continue;

        m_pAnimation[nPriority]->FrameMove();

        if (m_nPriority == nPriority)
            m_pAnimation[nPriority]->GetCurFrame(m_UpdateAniInfo.dwFrame, m_UpdateAniInfo.dwFrameTo, m_UpdateAniInfo.fWeight);
    }
}
