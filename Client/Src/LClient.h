#include <Windows.h>
#include "LAssert.h"
#include "LExports.h"
#include "L3DInterface.h"

class L3DENGINE_CLASS IL3DEngine;

class LScene;
class LInput;

class LClient
{
public:
    friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    bool Init(HINSTANCE hInstance);
    bool Update();
    void Uninit();

    BOOL IsActive();

    HRESULT ShowFPS(float fDeltaTime);
protected:
private:
    float m_fLastTime = 0.f;
    DWORD m_nLastFrame = 0;
    float m_fTimeElapsed = 0.f;
    int m_nGameLoop = 0;

    BOOL m_bActive = true;

    std::unique_ptr<LScene> m_pScene {};
    std::unique_ptr<LInput> m_pInput {};

    LRESULT	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void InitL3DWindow(HINSTANCE hInstance, L3D_WINDOW_PARAM& m_WindowParam);

    void _UpdateMessage();
};

extern std::unique_ptr<LClient> g_pClient;