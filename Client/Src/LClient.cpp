#include "LClient.h"
#include "L3DInterface.h"

#include "Object/LCharacter.h"
#include "World/LScene.h"
#include "World/LCamera.h"

extern LClient* g_pClient = new LClient;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return g_pClient->MsgProc(hWnd, msg, wParam, lParam);
}

HRESULT LClient::Init(HINSTANCE hInstance)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    L3D_WINDOW_PARAM WindowParam;
    LScene* pCurScene = nullptr;
    LCharacter* pCharacter = nullptr;

    IL3DEngine* piEngine = nullptr;
    ILScene* piScene = nullptr;

    WindowParam.x = 100;
    WindowParam.y = 100;
    WindowParam.Width = 800;
    WindowParam.Height = 600;
    WindowParam.bWindow = TRUE;
    WindowParam.lpszClassName = TEXT("LDirect3D");
    WindowParam.lpszWindowName = TEXT("L3D Engine");

    InitL3DWindow(hInstance, WindowParam);

    LObjectMgr::Instance().Init(hInstance, WindowParam);

    m_pScene = new LScene;
    m_pScene->Create("Res/maps/唐门登陆界面/唐门登陆界面.jsonmap");

    m_fLastTime = (float)timeGetTime();

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT LClient::Update()
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    float fCurTime = 0;
    float fDeltaTime = 0;

    fCurTime = (float)timeGetTime();
    fDeltaTime = (fCurTime - m_fLastTime) * 0.001f;

    m_pScene->Update(fDeltaTime);
    LObjectMgr::Instance().Update(fDeltaTime);

    hr = ShowFPS(fDeltaTime);
    HRESULT_ERROR_EXIT(hr);

    m_fLastTime = fCurTime;

    hResult = S_OK;
Exit0:
    return hResult;
}

void LClient::Uninit()
{
    SAFE_DELETE(m_pScene);
}

BOOL LClient::IsActive()
{
    return LObjectMgr::Instance().IsActive();
}

HRESULT LClient::ShowFPS(float fDeltaTime)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    WCHAR wszFPS[LENGIEN_FONT_STRING_MAX];

    m_nFrame++;
    m_fTimeElapsed += fDeltaTime;

    if (m_fTimeElapsed >= 1.0f)
    {
        swprintf(wszFPS, LENGIEN_FONT_STRING_MAX, TEXT("FPS:%.2f"), m_nFrame / m_fTimeElapsed);

        m_fTimeElapsed = 0.f;
        m_nFrame = 0;
    }
    hResult = S_OK;
Exit0:
    return hResult;
}


LRESULT LClient::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        if (m_pScene)
            m_pScene->m_pCamera->SetSightDistance(GET_WHEEL_DELTA_WPARAM(wParam) * -0.1f);
        break;
    }
    case WM_KEYDOWN:
    {
        if (wParam == VK_ESCAPE)
            DestroyWindow(hWnd);
        break;
    }
    default:
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void LClient::InitL3DWindow(HINSTANCE hInstance, L3D_WINDOW_PARAM& Param)
{
    BOOL bRetCode = FALSE;
    HRESULT hResult = E_FAIL;
    WNDCLASSEX wndClassEx;

    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
    wndClassEx.lpfnWndProc = (WNDPROC)WndProc;
    wndClassEx.cbClsExtra = 0;
    wndClassEx.cbWndExtra = 0;
    wndClassEx.hInstance = hInstance;
    wndClassEx.hIcon = ::LoadIcon(NULL, IDI_WINLOGO);
    wndClassEx.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wndClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClassEx.lpszMenuName = NULL;
    wndClassEx.lpszClassName = Param.lpszClassName;
    wndClassEx.hIconSm = NULL;

    RegisterClassEx(&wndClassEx);
    Param.wnd = CreateWindow(Param.lpszClassName, Param.lpszWindowName, WS_OVERLAPPEDWINDOW,
        Param.x, Param.y, Param.Width, Param.Height, NULL, NULL, hInstance, NULL);

    ShowWindow(Param.wnd, SW_SHOWDEFAULT);
    UpdateWindow(Param.wnd);
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    g_pClient->Init(hInstance);

    while (g_pClient->IsActive())
        g_pClient->Update();

    g_pClient->Uninit();

    return 1;
}