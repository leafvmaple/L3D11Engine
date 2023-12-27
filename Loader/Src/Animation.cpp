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

    pSource->pFlag = new int[pSource->nBonesCount];
    memset(pSource->pFlag, 0, sizeof(int) * pSource->nBonesCount);

    InitBoneRTS.resize(pSource->nBonesCount);
    for (int i = 0; i < pSource->nBonesCount; i++)
    {
        auto& flag = pInitBoneRTS[i].byFlag;
        if (flag == ONLY_ROTATION || flag == ONLY_ROTATION_TRANSLATION || flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & ROTATION)
            InitBoneRTS[i].Rotation = pInitBoneRTS[i].Rotation;

        if (flag == ONLY_ROTATION_TRANSLATION || flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & TRANSLATION)
            InitBoneRTS[i].Translation = pInitBoneRTS[i].Translation;

        if (flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & AFFINESCALE) {
            pSource->pFlag[i] |= BONE_FLAG_AFFLINE;
            InitBoneRTS[i].Scale = pInitBoneRTS[i].Scale;
        }
        else if (flag & SCALE) {
            InitBoneRTS[i].Scale = pInitBoneRTS[i].Scale;
            if (flag & SROTATION)
                InitBoneRTS[i].SRotation = pInitBoneRTS[i].SRotation;
        }
    }

    pSource->pBoneRTS = new RTS* [pSource->nBonesCount];
    for (int i = 0; i < pSource->nBonesCount; i++)
    {
        auto nAnimationIndex = pBoneToAnimationIndies[i];

        pSource->pBoneRTS[i] = new RTS[pSource->nFrameCount];
        for (int j = 0; j < pSource->nFrameCount; j++)
            pSource->pBoneRTS[i][j] = InitBoneRTS[i];

        if (nAnimationIndex >= 0)
        {
            _CompressRotation* pSRotation = nullptr;
            _CompressRotation* pRotation = nullptr;
            XMFLOAT3* pTranslation = nullptr;
            float* pAffineScale = nullptr;
            XMFLOAT3* pScale = nullptr;

            assert(nAnimationIndex < nRealAnimationBones);
            auto& flag = pAnimationIndexFlag[nAnimationIndex];

            if (flag == ONLY_ROTATION || flag == ONLY_ROTATION_TRANSLATION || flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & ROTATION)
                pReader->Convert(pRotation, pSource->nFrameCount);

            if (flag == ONLY_ROTATION_TRANSLATION || flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & TRANSLATION)
                pReader->Convert(pTranslation, pSource->nFrameCount);

            if (flag == ONLY_ROTATION_TRANSLATION_AFFINESCALE || flag & AFFINESCALE)
                pReader->Convert(pAffineScale, pSource->nFrameCount);
            else if (flag & SCALE) {
                pReader->Convert(pScale, pSource->nFrameCount);
                if (flag & SROTATION)
                    pReader->Convert(pSRotation, pSource->nFrameCount);
            }

            for (int j = 0; j < pSource->nFrameCount; j++)
            {
                auto& rts = pSource->pBoneRTS[i][j];

                if (pRotation)
                    XMStoreFloat4(&rts.Rotation, XMVectorSet(
                        pRotation->wzyx[0] * CRPRECISION, pRotation->wzyx[1] * CRPRECISION, pRotation->wzyx[2] * CRPRECISION, pRotation->wzyx[3] * CRPRECISION));

                if (pSRotation)
                    XMStoreFloat4(&rts.SRotation, XMVectorSet(
                        pSRotation->wzyx[0] * CRPRECISION, pSRotation->wzyx[1] * CRPRECISION, pSRotation->wzyx[2] * CRPRECISION, pSRotation->wzyx[3] * CRPRECISION));

                if (pTranslation)
                    rts.Translation = pTranslation[j];
                if (pScale)
                    rts.Scale = pScale[j];
                else if (pAffineScale)
                    rts.Scale = XMFLOAT3(pAffineScale[j], pAffineScale[j], pAffineScale[j]);
            }
        }
    }

    return;
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
