#pragma once
#include <Windows.h>

class LScene;
class LCamera;

class LInput
{
public:
    void Init(HWND pWnd);
    void ProcessInput(UINT& uMsg, WPARAM& wParam, LPARAM& lParam, LScene* pScene);

private:
    HWND m_pWnd = nullptr;
    BOOL m_bLButtonDown = false;
    BOOL m_bKeyDown = false;

    int m_nCursorX = 0;
    int m_nCursorY = 0;

    float m_fStepX = 0.1f;
    float m_fStepY = 0.05f;

    float m_fUpStep = 5.0f;

    void _CameraTargetUp(LCamera* pCamera, float y);
};