#pragma once
#include <Windows.h>

class LScene;

class LInput
{
public:
    void ProcessInput(UINT& uMsg, WPARAM& wParam, LPARAM& lParam, LScene* pScene);

private:
    BOOL m_bLButtonDown = false;

    int m_nCursorX = 0;
    int m_nCursorY = 0;

    float m_fStepX = 0.1f;
    float m_fStepY = 0.05f;
};