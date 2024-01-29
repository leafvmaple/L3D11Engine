#include "LClient.h"
#include "L3DInterface.h"

#include "Object/LCharacter.h"
#include "World/LScene.h"
#include "World/LCamera.h"
#include "World/LInput.h"

extern LClient* g_pClient = new LClient;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return g_pClient->MsgProc(hWnd, msg, wParam, lParam);
}

bool LClient::Init(HINSTANCE hInstance)
{
    L3D_WINDOW_PARAM WindowParam;
    IL3DEngine* piEngine = IL3DEngine::Instance();;

    WindowParam.x = 100;
    WindowParam.y = 100;
    WindowParam.Width = 800;
    WindowParam.Height = 600;
    WindowParam.bWindow = TRUE;
    WindowParam.lpszClassName = TEXT("LDirect3D");
    WindowParam.lpszWindowName = TEXT("L3D Engine");

    InitL3DWindow(hInstance, WindowParam);

    CHECK_BOOL(piEngine->Init(hInstance, WindowParam));

    m_pScene = new LScene;
    CHECK_BOOL(m_pScene->Create("data/source/maps/µ¾Ïã´å/µ¾Ïã´å.jsonmap"));

    m_pInput = new LInput;
    CHECK_BOOL(m_pInput);
    m_pInput->Init(WindowParam.wnd);

    m_fLastTime = piEngine->GetTime();

    return true;
}

bool LClient::Update()
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    DWORD nFrame = 0;
    DWORD nDeltaFrame = 0;
    float fTime = 0;
    float fDeltaTime = 0;
    IL3DEngine* piEngine = IL3DEngine::Instance();

    nFrame = piEngine->GetFrame(); // ms
    fTime = piEngine->GetTime();

    if (nFrame > m_nLastFrame)
        m_pScene->FrameMove(nFrame);

    _UpdateMessage();

    fDeltaTime = (fTime - m_fLastTime) * 0.001f;
    m_pScene->Update(fDeltaTime);
    piEngine->Update(fDeltaTime);

    hr = ShowFPS(fDeltaTime);
    HRESULT_ERROR_EXIT(hr);

    m_fLastTime = fTime;
    m_nLastFrame = nFrame;

    hResult = S_OK;
Exit0:
    return hResult;
}

void LClient::Uninit()
{
    SAFE_DELETE(m_pScene);

    IL3DEngine::Destroy();
}

BOOL LClient::IsActive()
{
    return m_bActive;
}

HRESULT LClient::ShowFPS(float fDeltaTime)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    WCHAR wszFPS[LENGIEN_FONT_STRING_MAX];

    m_nGameLoop++;
    m_fTimeElapsed += fDeltaTime;

    if (m_fTimeElapsed >= 1.0f)
    {
        swprintf(wszFPS, LENGIEN_FONT_STRING_MAX, TEXT("FPS:%.2f"), m_nGameLoop / m_fTimeElapsed);

        m_fTimeElapsed = 0.f;
        m_nGameLoop = 0;
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
        m_bActive = false;
        break;
    }
    
    case WM_QUIT:
    {
        m_bActive = false;
        break;
    }
    default:
    {
        m_pInput->ProcessInput(uMsg, wParam, lParam, m_pScene);
        break;
    }
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


void LClient::_UpdateMessage()
{
    MSG Msg;

    ::ZeroMemory(&Msg, sizeof(MSG));

    while (::PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&Msg);
        ::DispatchMessage(&Msg);
    }
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    g_pClient->Init(hInstance);

    while (g_pClient->IsActive())
        g_pClient->Update();

    g_pClient->Uninit();

    return 1;
}