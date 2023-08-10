#include "stdafx.h"
#include "L3DEngineTimer.h"
#include <Windows.h>

void L3DEngineTimer::Update()
{
    m_nNowTime = timeGetTime();
}

L3DEngineTimer g_Timer;