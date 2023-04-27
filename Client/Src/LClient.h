#include <Windows.h>
#include "LAssert.h"
#include "LExports.h"
#include "L3DInterface.h"
#include "LObjectMgr.h"

class L3DENGINE_CLASS IL3DEngine;

class LClient
{
public:
    friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    LClient();
    ~LClient();

    HRESULT Init(HINSTANCE hInstance);
    HRESULT Update();
    void Uninit();

    BOOL IsActive();

    HRESULT ShowFPS(float fDeltaTime);
protected:
private:
    float m_fLastTime;
    float m_fTimeElapsed;
    int m_nFrame;

    LObjectMgr* m_pObjectMgr;

    LRESULT	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void InitL3DWindow(HINSTANCE hInstance, L3D_WINDOW_PARAM& m_WindowParam);
};

extern LClient* g_pClient;