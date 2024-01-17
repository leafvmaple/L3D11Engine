#include "LFrameData.h"

#include <windows.h>

void LFrameData::Init()
{
    // Local 128 frames
    m_FrameArray.resize(128);
}

void LFrameData::UpdateFramesData(int nGameLoop, CHARACTER_MOVE_POSITION* pUpdateData)
{
    int nIndex = GetFrameIndex(nGameLoop);
    if (m_FrameArray[nIndex].nGameLoop != nGameLoop)
        nIndex = GetMaxGameLoopFrameIndex();

    for (int i = 0; i < 88; i++)
    {
        m_FrameArray[nIndex].nGameLoop = nGameLoop + i;
        m_FrameArray[nIndex].PositionData = *pUpdateData;

        nIndex = (nIndex + 1) % m_FrameArray.size();
    }
}

void LFrameData::Interpolate(IL3DEngine* p3DEngine, ILScene* p3DScene, float fTime)
{
    int nNextIndex = 0;
    XMFLOAT3 vPosition;
    XMFLOAT3 vPositionL;
    XMFLOAT3 vPositionR;

    DWORD nFrame = p3DEngine->GetFrame();

    int nIndex = GetFrameIndex(nFrame);

    if (m_FrameArray[nIndex].nGameLoop != nFrame)
    {
        nIndex = GetMaxGameLoopFrameIndex();
        nNextIndex = nIndex;
    }
    else
    {
        nNextIndex = (nIndex + 1) % m_FrameArray.size();
        if (m_FrameArray[nNextIndex].nGameLoop != m_FrameArray[nIndex].nGameLoop + 1)
            nNextIndex = nIndex;
    }

    float fTimeL = p3DEngine->FrameToTime(m_FrameArray[nIndex].nGameLoop);
    float fTimeR = p3DEngine->FrameToTime(m_FrameArray[nNextIndex].nGameLoop);

    if (fTimeR > fTimeL)
    {
        CHARACTER_MOVE_POSITION& PositionDataL = m_FrameArray[nIndex].PositionData;
        CHARACTER_MOVE_POSITION& PositionDataR = m_FrameArray[nNextIndex].PositionData;

        p3DScene->LogicalToScene(vPositionL, PositionDataL.nX, PositionDataL.nY, PositionDataL.nZ);
        p3DScene->LogicalToScene(vPositionR, PositionDataR.nX, PositionDataR.nY, PositionDataR.nZ);

        float fWeight = (p3DEngine->GetTime() - fTimeL) / (fTimeR - fTimeL);
        XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vPositionL), XMLoadFloat3(&vPositionR), fWeight));

        m_Interpolate.State = PositionDataR.State;
        m_Interpolate.vPosition = vPosition;
    }
}

void LFrameData::GetPosition(XMFLOAT3& vPosition)
{
    vPosition = m_Interpolate.vPosition;
}

float LFrameData::GetFaceDirection()
{
    return m_Interpolate.fFaceDirection;
}

int LFrameData::GetFrameIndex(int nGameLoop)
{
    for (int i = 0; i < m_FrameArray.size(); ++i)
        if (m_FrameArray[i].nGameLoop == nGameLoop)
            return i;
    return 0;
}

int LFrameData::GetMaxGameLoopFrameIndex()
{
    int nIndex = 0;
    for (int i = 1; i < m_FrameArray.size(); ++i)
        if (m_FrameArray[i].nGameLoop > m_FrameArray[nIndex].nGameLoop)
            nIndex = i;

    return nIndex;
}
