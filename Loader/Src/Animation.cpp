#include "Animation.h"
#include "LAssert.h"
#include "LFileReader.h"

inline unsigned int FrameToTime(DWORD dwFrame, float fFrameLength)
{
    return (unsigned int)(fFrameLength * dwFrame + 0.5f);
}

void _LoadAnimation(LBinaryReader* pReader, ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource)
{
    LBinaryReader Reader;
    _BONE_ANI* pBoneAni = nullptr;

    pReader->Convert(pBoneAni);

    pSource->nBonesCount = pBoneAni->dwNumBones;
    pSource->nFrameCount = pBoneAni->dwNumFrames;
    pSource->fFrameLength = pBoneAni->fFrameLength;
    pSource->nAnimationLength = FrameToTime(pBoneAni->dwNumFrames - 1, pBoneAni->fFrameLength);

    pSource->pBoneNames = new char[pSource->nBonesCount][ANI_STRING_SIZE];
    for (int i = 0; i < pSource->nBonesCount; i++)
        pReader->Copy(pSource->pBoneNames[i], ANI_STRING_SIZE);

    pSource->pBoneRTS = new RTS * [pSource->nBonesCount];
    for (int i = 0; i < pSource->nBonesCount; i++)
    {
        pSource->pBoneRTS[i] = new RTS[pSource->nFrameCount];
        pReader->Copy(pSource->pBoneRTS[i], pSource->nFrameCount);

        // Rotation is Inverse
        for (int j = 0; j < pSource->nFrameCount; j++)
        {
            auto& rts = pSource->pBoneRTS[i][j];
            XMStoreFloat4(&rts.Rotation, XMQuaternionInverse(XMLoadFloat4(&rts.Rotation)));
        }
    }
}

enum _RTSV2Flag
{
    NONE = 0,
    ONLY_ROTATION = 1,
    ONLY_ROTATION_TRANSLATION = 2,
    ONLY_ROTATION_TRANSLATION_AFFINESCALE = 3,

    ROTATION = 1 << 2,
    TRANSLATION = 1 << 3,
    AFFINESCALE = 1 << 4,
    SCALE = 1 << 5,
    SROTATION = 1 << 6,
    SIGN = 1 << 7
};

const float CRPRECISION = 1.f / SHRT_MAX;
void _LoadAnimationV2(LBinaryReader* pReader, ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource)
{
    LBinaryReader Reader;
    _BONE_ANI_V2* pBoneAni = nullptr;
    int* pBoneToAnimationIndies = nullptr;
    BYTE* pAnimationIndexFlag = nullptr;
    _RTSV2* pInitBoneRTS = nullptr;
    int bBoneName = false;
    int nRealAnimationBones = 0;
    std::vector<RTS> InitBoneRTS;

    pReader->Convert(pBoneAni);

    pSource->nBonesCount = pBoneAni->dwNumBones;
    pSource->nFrameCount = pBoneAni->dwNumFrames;
    pSource->fFrameLength = pBoneAni->fFrameLength;
    pSource->nAnimationLength = FrameToTime(pBoneAni->dwNumFrames - 1, pBoneAni->fFrameLength);

    bBoneName = pBoneAni->bHasBoneName;
    nRealAnimationBones = pBoneAni->dwRealAniBones;
    
    assert(bBoneName);
    assert(nRealAnimationBones > 0);

    pSource->pBoneNames = new char[pSource->nBonesCount][ANI_STRING_SIZE];
    for (int i = 0; i < pSource->nBonesCount; i++)
        pReader->Copy(pSource->pBoneNames[i], ANI_STRING_SIZE);

    pReader->Convert(pInitBoneRTS, pSource->nBonesCount);
    pReader->Convert(pBoneToAnimationIndies, pSource->nBonesCount);
    pReader->Convert(pAnimationIndexFlag, nRealAnimationBones);

    pSource->pAffline = new int[pSource->nBonesCount];
    for (int i = 0; i < pSource->nBonesCount; i++)
        pSource->pAffline[i] = (pInitBoneRTS[i].byFlag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || pInitBoneRTS[i].byFlag & ROTATION);

    std::vector<std::vector<RTS>> AnimationIndexRTS(nRealAnimationBones);
    for (int i = 0; i < nRealAnimationBones; i++)
    { 
        _CompressRotation* pRotation = nullptr;
        XMFLOAT3* pTranslation = nullptr;
        float* pAffineScale = nullptr;
        XMFLOAT3* pScale = nullptr;

        AnimationIndexRTS[i].resize(pSource->nFrameCount);

        auto& flag = pAnimationIndexFlag[i];

        if (flag == ONLY_ROTATION || flag == ONLY_ROTATION_TRANSLATION || flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & ROTATION)
            pReader->Convert(pRotation, pSource->nFrameCount);

        if (flag == ONLY_ROTATION_TRANSLATION || flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & TRANSLATION)
            pReader->Convert(pTranslation, pSource->nFrameCount);

        if (flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & AFFINESCALE)
            pReader->Convert(pAffineScale, pSource->nFrameCount);
        else if (flag & SCALE)
            pReader->Convert(pScale, pSource->nFrameCount);

        for (int j = 0; j < pSource->nFrameCount; j++)
        {
            auto& rts = AnimationIndexRTS[i][j];

            if (pRotation)
                XMStoreFloat4(&rts.Rotation, XMQuaternionConjugate(XMVectorSet(
                    pRotation->wzyx[0] * CRPRECISION, pRotation->wzyx[1] * CRPRECISION, pRotation->wzyx[2] * CRPRECISION, pRotation->wzyx[3] * CRPRECISION)));

            if (pTranslation)
                rts.Translation = pTranslation[j];

            if (pScale)
                rts.Scale = pScale[j];
            else if (pAffineScale)
                rts.Scale = XMFLOAT3(pAffineScale[j], pAffineScale[j], pAffineScale[j]);
        }
    }

    pSource->pBoneRTS = new RTS* [pSource->nBonesCount];
    for (int i = 0; i < pSource->nBonesCount; i++)
    {
        auto nAnimationIndex = pBoneToAnimationIndies[i];

        pSource->pBoneRTS[i] = new RTS[pSource->nFrameCount];

        if (nAnimationIndex > 0)
        {
            assert(nAnimationIndex < nRealAnimationBones);
            memcpy(pSource->pBoneRTS[i], AnimationIndexRTS[nAnimationIndex].data(), pSource->nFrameCount * sizeof(RTS));
        }
        else
        {
            for (int j = 0; j < pSource->nFrameCount; j++)
            {
                pSource->pBoneRTS[i][j].Rotation = pInitBoneRTS[i].Rotation;
                pSource->pBoneRTS[i][j].Translation = pInitBoneRTS[i].Translation;
                pSource->pBoneRTS[i][j].Scale = pInitBoneRTS[i].Scale;
                pSource->pBoneRTS[i][j].SRotation = pInitBoneRTS[i].SRotation;
            }
        }
    }

    //pSource->pBoneRTS = new RTS * [pSource->nBonesCount];
    //_RTSV2* pRTSV2 = new _RTSV2[pSource->nFrameCount];
    //for (int i = 0; i < pSource->nBonesCount; i++)
    //{
    //    pReader->Copy(pRTSV2, pSource->nFrameCount);

    //    pSource->pBoneRTS[i] = new RTS[pSource->nFrameCount];
    //    for (int j = 0; j < pSource->nFrameCount; j++)
    //    {
    //        auto& rts = pSource->pBoneRTS[i][j];
    //        auto rtsV2 = pRTSV2[j];

    //        rts.Rotation = XMFLOAT4(0, 0, 0, 1.0f);
    //        rts.SRotation = XMFLOAT4(0, 0, 0, 1.0f);
    //        rts.Translation = XMFLOAT3(0, 0, 0);
    //        rts.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    //        rts.Sign = 1.0f;

    //        switch (rtsV2.byFlag)
    //        {
    //        case ONLY_ROTATION_TRANSLATION_AFFINESCALE:
    //            rts.Scale = XMFLOAT3(rtsV2.Scale.x, rtsV2.Scale.x, rtsV2.Scale.x);
    //            [[fallthrough]];
    //        case ONLY_ROTATION_TRANSLATION:
    //            rts.Translation = rtsV2.Translation;
    //            [[fallthrough]];
    //        case ONLY_ROTATION:
    //            XMStoreFloat4(&rts.Rotation, XMQuaternionConjugate(XMLoadFloat4(&rtsV2.Rotation)));
    //            break;
    //        default:
    //            if (rtsV2.byFlag & ROTATION)
    //                XMStoreFloat4(&rts.Rotation, XMQuaternionConjugate(XMLoadFloat4(&rtsV2.Rotation)));
    //            if (rtsV2.byFlag & TRANSLATION)
    //                rts.Translation = rtsV2.Translation;
    //            if (rtsV2.byFlag & SCALE)
    //                rts.Scale = rtsV2.Scale;
    //            if (rtsV2.byFlag & AFFINESCALE)
    //                rts.Scale = XMFLOAT3(rtsV2.Scale.x, rtsV2.Scale.x, rtsV2.Scale.x);
    //            if (rtsV2.byFlag & SIGN)
    //                rts.Sign = -1.0f;
    //            if (rtsV2.byFlag & SROTATION && rtsV2.byFlag & SCALE) {
    //                XMStoreFloat4(&rts.SRotation, XMQuaternionConjugate(XMLoadFloat4(&rtsV2.SRotation)));
    //            }
    //        }
    //    }
    //}
    //SAFE_DELETE_ARRAY(pRTSV2);
}

void LoadAnimation(ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource)
{
    LBinaryReader Reader;

    _ANI_FILE_HEADER* pHead = nullptr;

    pSource = new ANIMATION_SOURCE;
    pSource->AddRef();

    Reader.Init(pDesc->szFileName);
    Reader.Convert(pHead);

    assert(pHead->dwType == ANIMATION_BONE_RTS);

    pSource->nAnimationType = pHead->dwType;

    if (pHead->dwMask == ANI_FILE_MASK)
        _LoadAnimation(&Reader, pDesc, pSource);
    else if (pHead->dwMask == ANI_FILE_MASK_VERVION2)
        _LoadAnimationV2(&Reader, pDesc, pSource);
    else
        assert(false);
}
