#include "stdafx.h"

#include <Windows.h>
#include <process.h>
#include <d3dcommon.h>
#include <strsafe.h>
#include <d3d11.h>

#include "L3DEngine.h"
#include "L3DFormat.h"

#include "Model/L3DModel.h"
#include "Model/L3DShader.h"

#include "Input/L3DInput.h"
#include "Camera/L3DCamera.h"
#include "Component/L3DWindow.h"

#include "L3DFormat.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

static const D3D_FEATURE_LEVEL FEATURE_LEVEL_ARRAY_0[] =
{
    D3D_FEATURE_LEVEL_11_0,
};

static const D3D_FEATURE_LEVEL FEATURE_LEVEL_ARRAY_1[] =
{
    D3D_FEATURE_LEVEL_11_1,
};

extern ID3D11Device* g_p3DDevice = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    L3DEngine* pEngine = dynamic_cast<L3DEngine*>(L3DEngine::Instance());
    return pEngine->MsgProc(hWnd, msg, wParam, lParam);
}

L3DEngine::L3DEngine()
: m_bActive(false)
, m_CurSampFilter(m_SampFilter[GRAPHICS_LEVEL_MAX])
{
    memset(&m_SampFilter, 0, sizeof(m_SampFilter));
    memset(&m_WindowParam, 0, sizeof(m_WindowParam));
}

L3DEngine::~L3DEngine()
{

}

HRESULT L3DEngine::Init(HINSTANCE hInstance, L3D_WINDOW_PARAM& WindowParam)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    BOOL nRetCode = false;
    HWND hWnd = NULL;
    UINT uCreateDeviceFlag = 0;

    m_WindowParam = WindowParam;

	hr = InitL3DWindow(&hWnd, hInstance);
	HRESULT_ERROR_EXIT(hr);

    hr = _SetupD3D();
    HRESULT_ERROR_EXIT(hr);

    hr = _CreateTargetWindow(hWnd);
    HRESULT_ERROR_EXIT(hr);

    hr = InitSamplerFilter();
    HRESULT_ERROR_EXIT(hr);

    hr = InitInput(hWnd, hInstance);
    HRESULT_ERROR_EXIT(hr);

    hr = InitCamera(m_WindowParam.Width, m_WindowParam.Height);
    HRESULT_ERROR_EXIT(hr);

    m_bActive = TRUE;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DEngine::Update(float fDeltaTime)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    SCENE_RENDER_OPTION RenderOption;

    hr = UpdateMessage();
    HRESULT_ERROR_EXIT(hr);

    hr = UpdateInput();
    HRESULT_ERROR_EXIT(hr);

    hr = UpdateCamera(fDeltaTime);
    HRESULT_ERROR_EXIT(hr);

    RenderOption.piDevice = m_Device.piDevice;
    RenderOption.piImmediateContext = m_Device.piImmediateContext;

    RenderOption.pShaderTable = m_pShaderTable;
    RenderOption.pCamera = m_pCamera;

    hr = m_pWindow->Update(m_Device.piDevice, RenderOption);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

BOOL L3DEngine::IsActive()
{
    return m_bActive;
}


ID3D11Device* L3DEngine::GetDevice() const
{
    return m_Device.piDevice;
}

HRESULT L3DEngine::AttachScene(L3DScene* pScene)
{
    return m_pWindow->AddScene(pScene);
}

HRESULT L3DEngine::Uninit()
{
    return S_OK;
}

LRESULT L3DEngine::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)  
    {
    case WM_DESTROY:
        PostQuitMessage(0);  
        break;
    case WM_MOUSEWHEEL:
        if (m_pCamera)
            m_pCamera->UpdateSightDistance(GET_WHEEL_DELTA_WPARAM(wParam) * -0.1f);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            DestroyWindow(hWnd);
        break;
    }  

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


HRESULT L3DEngine::InitL3DWindow(HWND* pWnd, HINSTANCE hInstance)
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
    wndClassEx.lpszClassName = m_WindowParam.lpszClassName;
    wndClassEx.hIconSm = NULL;

    bRetCode = RegisterClassEx(&wndClassEx);
    BOOL_ERROR_EXIT(bRetCode);

    *pWnd = CreateWindow(m_WindowParam.lpszClassName, m_WindowParam.lpszWindowName, WS_OVERLAPPEDWINDOW,
        m_WindowParam.x, m_WindowParam.y, m_WindowParam.Width, m_WindowParam.Height, NULL, NULL, hInstance, NULL);
    BOOL_ERROR_EXIT(*pWnd);

    ShowWindow(*pWnd, SW_SHOWDEFAULT);

    bRetCode = UpdateWindow(*pWnd);
    BOOL_ERROR_EXIT(bRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DEngine::InitSamplerFilter()
{
    HRESULT hr = E_FAIL;

    for (int i = 0; i < GRAPHICS_LEVEL_COUNT; i++)
    {
        hr = m_Device.piDevice->CreateSamplerState(&m_SampFilter[i], &m_pSamplerState[i]);
        HRESULT_ERROR_CONTINUE(hr);
    }

    return S_OK;
}


HRESULT L3DEngine::InitInput(HWND hWnd, HINSTANCE hInstance)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    m_pInput = new L3DInput;
    BOOL_ERROR_EXIT(m_pInput);

    hr = m_pInput->Init(hWnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DEngine::InitCamera(float fWidth, float fHeight)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    m_pCamera = new L3DCamera;
    BOOL_ERROR_EXIT(m_pCamera);

    hr = m_pCamera->Init(fWidth, fHeight);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT L3DEngine::_SetupD3D()
{
    HRESULT hr = E_FAIL;
    HRESULT hReusult = E_FAIL;

    hr = _CreateDeivice();
    HRESULT_ERROR_EXIT(hr);

    m_pShaderTable = CreateShaderTable(m_Device.piDevice);
    BOOL_ERROR_EXIT(m_pShaderTable);

    hReusult = S_OK;
Exit0:
    return hReusult;
}

HRESULT L3DEngine::_CreateTargetWindow(HWND hWnd)
{
    HRESULT hResult = E_FAIL;

    m_pWindow = L3DCreateWindow(m_Device.piDevice, hWnd);
    BOOL_ERROR_EXIT(m_pWindow);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DEngine::_CreateDeivice()
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	int nRetCode = 0;
	UINT uCreateDeviceFlag = 0;

	// Check support for SSE2
	nRetCode = XMVerifyCPUSupport();
	BOOL_ERROR_EXIT(nRetCode);

#if defined(DEBUG) || defined(_DEBUG)  
	uCreateDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, uCreateDeviceFlag,
		FEATURE_LEVEL_ARRAY_0, _countof(FEATURE_LEVEL_ARRAY_0),
		D3D11_SDK_VERSION,
		&m_Device.piDevice, &m_Device.eFeatureLevel, &m_Device.piImmediateContext
	);

	if (FAILED(hr))
	{
		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, uCreateDeviceFlag,
			FEATURE_LEVEL_ARRAY_1, _countof(FEATURE_LEVEL_ARRAY_1),
			D3D11_SDK_VERSION,
			&m_Device.piDevice, &m_Device.eFeatureLevel, &m_Device.piImmediateContext
		);
	}
	HRESULT_ERROR_EXIT(hr);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT L3DEngine::UpdateMessage()
{
    MSG Msg;

    ::ZeroMemory(&Msg, sizeof(MSG));

    while(::PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&Msg);
        ::DispatchMessage(&Msg);
    }

    if (Msg.message == WM_QUIT)
        m_bActive = FALSE;

    return S_OK;
}


HRESULT L3DEngine::UpdateInput()
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    BOOL_ERROR_EXIT(m_pInput);

    hr = m_pInput->Update();
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DEngine::UpdateCamera(float fDeltaTime)
{
    HRESULT hResult = E_FAIL;

    float fYaw = 0.f;
    float fPitch = 0.f;
    float fRoll = 0.f;

    BOOL_ERROR_EXIT(m_pInput);
    BOOL_ERROR_EXIT(m_pCamera);

    if (m_pInput->IsMouseButtonDown(0))
    {
        fYaw -= (m_pInput->MouseDX()) * 0.01f;
        fPitch -= (m_pInput->MouseDY()) * 0.01f;
    }

    if (m_pInput->IsKeyDown(DIK_LEFT))
        fYaw -= 0.001f;

    if (m_pInput->IsKeyDown(DIK_RIGHT))
        fYaw += 0.001f;

    if (m_pInput->IsKeyDown(DIK_UP))
        fPitch += 0.001f;

    if (m_pInput->IsKeyDown(DIK_DOWN))
        fPitch -= 0.001f;

    if (fYaw || fPitch || fRoll)
        m_pCamera->UpdateYawPitchRoll(fYaw, fPitch, fRoll);

    hResult = S_OK;
Exit0:
    return hResult;
}
