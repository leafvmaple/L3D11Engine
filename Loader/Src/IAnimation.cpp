#include "IAnimation.h"
#include "LFileReader.h"

unsigned int FrameToTime(DWORD dwFrame, float fFrameLength)
{
	return (unsigned int)(fFrameLength * dwFrame + 0.5f);
}

void LoadFromFile(ANIMATION_DESC* pDesc, ANIMATION_SOURCE*& pSource)
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

	pSource->pBoneNames = new char[pSource->nBonesCount][MAX_PATH];
	for (int i = 0; i < pSource->nBonesCount; i++)
	{
		char* pszBoneName = nullptr;
		Reader.Convert(pszBoneName, ANI_STRING_SIZE);
		strcpy(pSource->pBoneNames[i], pszBoneName);
	}

	/*m_BoneRTS.resize(m_dwNumBone);
	for (int i = 0; i < m_dwNumBone; i++)
	{
		m_BoneRTS[i].resize(m_dwNumFrames);
		Reader.Copy(m_BoneRTS[i].data(), m_dwNumFrames);
	}

	m_szName = szAnimation;
	*/
}
