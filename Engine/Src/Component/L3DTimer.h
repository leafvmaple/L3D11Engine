#pragma once

class L3DTimer
{
public:
    void Update();

    unsigned int GetNowTime() { return m_nNowTime; };

private:
    unsigned int m_nNowTime = 0;
};

extern L3DTimer g_Timer;