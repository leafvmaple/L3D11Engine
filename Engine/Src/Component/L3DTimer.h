#pragma once

#include "L3DInterface.h"

class L3DTimer
{
public:
    void Init();
    void Update();

    unsigned int GetNowTime() { return m_nTime; }
    float GetTime() { return (float)(m_nTime - m_nStartTime); }
    DWORD GetFrame() { return (m_nTime - m_nStartTime) / (1000.f / GAME_FPS); }

private:
    unsigned int m_nTime = 0;
    unsigned int m_nStartTime = 0;
};

extern L3DTimer g_Timer;