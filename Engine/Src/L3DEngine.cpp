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
#include "State/L3DState.h"

#include "Camera/L3DCamera.h"
#include "Component/L3DWindow.h"
#include "Component/L3DTimer.h"
#include "Component/L3DLog.h"

#include "Render/L3DMaterialSystem.h"

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

L3DEngine::L3DEngine()
: m_CurSampFilter(m_SampFilter[GRAPHICS_LEVEL_MAX])
{
    memset(&m_WindowParam, 0, sizeof(m_WindowParam));
}

L3DEngine::~L3DEngine()
{
}

HRESULT L3DEngine::Init(HINSTANCE hInstance, L3D_WINDOW_PARAM& WindowParam)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    m_WindowParam = WindowParam;

    m_pLog = new L3DLog;
    BOOL_ERROR_EXIT(m_pLog);

    hr = _SetupD3D();
    HRESULT_ERROR_EXIT(hr);

    hr = _CreateTargetWindow(WindowParam.wnd);
    HRESULT_ERROR_EXIT(hr);

    hr = InitSamplerFilter();
    HRESULT_ERROR_EXIT(hr);

    g_pMaterialSystem->Init(m_Device.piDevice);

    // Temp Ç¿ÖÆË¢ÐÂ
    g_Timer.Init();

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DEngine::Update(float fDeltaTime)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    SCENE_RENDER_OPTION RenderOption;

    g_Timer.Update();

    RenderOption.piDevice = m_Device.piDevice;
    RenderOption.piImmediateContext = m_Device.piImmediateContext;

    RenderOption.pShaderTable = m_pShaderTable;
    RenderOption.pStateTable = m_pStateTable;

    m_pWindow->BeginPaint(m_Device.piDevice, RenderOption);
    m_pWindow->EndPaint(m_Device.piDevice, RenderOption);
    m_pWindow->Present();

    hResult = S_OK;
Exit0:
    return hResult;
}

ID3D11Device* L3DEngine::GetDevice() const
{
    return m_Device.piDevice;
}

DWORD L3DEngine::GetFrame() const
{
    return g_Timer.GetFrame();
}

float L3DEngine::GetTime() const
{
    return g_Timer.GetTime();
}

HRESULT L3DEngine::AttachScene(L3DScene* pScene)
{
    return m_pWindow->AddScene(pScene);
}

HRESULT L3DEngine::Uninit()
{
    SAFE_DELETE(m_pLog);

    return S_OK;
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

HRESULT L3DEngine::_SetupD3D()
{
    _CreateDeivice();

    m_pShaderTable = CreateShaderTable(m_Device.piDevice);
    m_pStateTable = CreateStateTable(m_Device.piDevice);

    return S_OK;
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
	HRESULT_ERROR_EXIT(hr);

	hResult = S_OK;
Exit0:
	return hResult;
}
