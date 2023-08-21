#include "stdafx.h"
#include "L3DEngineTimer.h"
#include <timeapi.h>

void L3DEngineTimer::Update()
{
    m_nNowTime = timeGetTime();
}

L3DEngineTimer g_Timer;