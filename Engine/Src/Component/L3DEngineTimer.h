#pragma once

class L3DEngineTimer
{
public:
    void Update();

    unsigned int GetNowTime() { return m_nNowTime; };

private:
    unsigned int m_nNowTime = 0;
};

extern L3DEngineTimer g_Timer;