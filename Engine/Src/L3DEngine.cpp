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

bool L3DEngine::Init(HINSTANCE hInstance, L3D_WINDOW_PARAM& WindowParam)
{
    m_WindowParam = WindowParam;

    m_pLog = new L3DLog;
    CHECK_BOOL(m_pLog);

    CHECK_BOOL(_SetupD3D());
    CHECK_BOOL(_CreateTargetWindow(WindowParam.wnd));
    CHECK_BOOL(_CreateSamplerFilter());

    g_pMaterialSystem->Init(m_Device.piDevice);

    // Temp Ç¿ÖÆË¢ÐÂ
    g_Timer.Init();

    return true;
}

void L3DEngine::Update(float fDeltaTime)
{
    SCENE_RENDER_OPTION RenderOption;

    g_Timer.Update();

    RenderOption.piDevice = m_Device.piDevice;
    RenderOption.piImmediateContext = m_Device.piImmediateContext;

    RenderOption.pShaderTable = m_pShaderTable;
    RenderOption.pStateTable = m_pStateTable;

    m_pWindow->BeginPaint(m_Device.piDevice, RenderOption);
    m_pWindow->EndPaint(m_Device.piDevice, RenderOption);
    m_pWindow->Present();
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

bool L3DEngine::AttachScene(L3DScene* pScene)
{
    m_pWindow->AddScene(pScene);

    return true;
}

void L3DEngine::Uninit()
{
    SAFE_DELETE(m_pLog);
}

bool L3DEngine::_SetupD3D()
{
    CHECK_BOOL(_CreateDeivice());
    CHECK_BOOL(CreateShaderTable(&m_pShaderTable, m_Device.piDevice));
    CHECK_BOOL(CreateStateTable(&m_pStateTable, m_Device.piDevice));

    return true;
}

bool L3DEngine::_CreateTargetWindow(HWND hWnd)
{
    CHECK_BOOL(Create3DWindow(&m_pWindow, m_Device.piDevice, hWnd));

    return true;
}

bool L3DEngine::_CreateDeivice()
{
    UINT uCreateDeviceFlag = 0;

    // Check support for SSE2
    CHECK_BOOL(XMVerifyCPUSupport());

#if defined(DEBUG) || defined(_DEBUG)  
    uCreateDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    CHECK_HRESULT(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, uCreateDeviceFlag,
        FEATURE_LEVEL_ARRAY_0, _countof(FEATURE_LEVEL_ARRAY_0),
        D3D11_SDK_VERSION,
        &m_Device.piDevice, &m_Device.eFeatureLevel, &m_Device.piImmediateContext
    ));

    return true;
}

bool L3DEngine::_CreateSamplerFilter()
{
    for (int i = 0; i < GRAPHICS_LEVEL_COUNT; i++)
        CHECK_HRESULT(m_Device.piDevice->CreateSamplerState(&m_SampFilter[i], &m_pSamplerState[i]));

    return true;
}
