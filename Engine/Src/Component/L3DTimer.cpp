#include "stdafx.h"
#include "L3DTimer.h"
#include <timeapi.h>

void L3DTimer::Update()
{
    m_nNowTime = timeGetTime();
}

L3DTimer g_Timer;