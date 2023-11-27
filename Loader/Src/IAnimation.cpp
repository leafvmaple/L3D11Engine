#include "IAnimation.h"
#include "LFileReader.h"

unsigned int FrameToTime(DWORD dwFrame, float fFrameLength)
{
	return (unsigned int)(fFrameLength * dwFrame + 0.5f);
}

void LoadAnimation(ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource)
{
	LBinaryReader Reader;

	_ANI_FILE_HEADER* pHead = nullptr;
	_BONE_ANI* pBoneAni = nullptr;

	pSource = new ANIMATION_SOURCE;
	pSource->AddRef();

	Reader.Init(pDesc->szFileName);
	Reader.Convert(pHead);

	assert(pHead->dwType == ANIMATION_BONE_RTS);
	assert(pHead->dwMask == ANI_FILE_MASK);

	pSource->nAnimationType = pHead->dwType;

	Reader.Convert(pBoneAni);

	pSource->nBonesCount = pBoneAni->dwNumBones;
	pSource->nFrameCount = pBoneAni->dwNumFrames;
	pSource->fFrameLength = pBoneAni->fFrameLength;
	pSource->nAnimationLength = FrameToTime(pBoneAni->dwNumFrames - 1, pBoneAni->fFrameLength);

	pSource->pBoneNames = new char[pSource->nBonesCount][ANI_STRING_SIZE];
	for (int i = 0; i < pSource->nBonesCount; i++)
		Reader.Copy(pSource->pBoneNames[i], ANI_STRING_SIZE);

	pSource->pBoneRTS = new RTS*[pSource->nBonesCount];
	for (int i = 0; i < pSource->nBonesCount; i++)
	{
		pSource->pBoneRTS[i] = new RTS[pSource->nFrameCount];
		Reader.Copy(pSource->pBoneRTS[i], pSource->nFrameCount);
	}
}
