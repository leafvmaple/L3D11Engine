#include "LFrameData.h"

#include <windows.h>

void LFrameData::AddPosition(int nX, int nY, int nZ)
{
    m_Position.nX += nX;
    m_Position.nY += nY;
    m_Position.nZ += nZ;

    m_Position.fUpdateTime = (float)timeGetTime();
}

void LFrameData::SetState(LState state)
{
    m_State = state;
}
