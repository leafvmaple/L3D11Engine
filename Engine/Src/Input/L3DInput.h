#pragma once
#include <dinput.h>
#include "L3DInterface.h"

class L3DInput
{
public:
    L3DInput();

    HRESULT Init(HWND hWnd, HINSTANCE hInstance, DWORD dwKeyboardCoopFlags, DWORD dwMouseCoopFlags);
    void Uninit();

    HRESULT Update();

    bool IsKeyDown(int nKey);
    bool IsMouseButtonDown(int nButton);
    float MouseDX();
    float MouseDY();
    float MouseDZ();

private:
    IDirectInput8* m_pDirectInput = nullptr;
    IDirectInputDevice8* m_KeyboardDevice = nullptr;
    IDirectInputDevice8* m_MouseDevice = nullptr;
    DIMOUSESTATE m_MouseState;
    char m_keyBuffer[256];
};
