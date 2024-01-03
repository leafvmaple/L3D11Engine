#include "stdafx.h"
#include "L3DTimer.h"
#include <timeapi.h>

void L3DTimer::Init()
{
    m_nTime = timeGetTime();
    m_nStartTime = m_nTime;
}

void L3DTimer::Update()
{
    m_nTime = timeGetTime();
}

L3DTimer g_Timer;