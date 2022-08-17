#include <Windows.h>
#include <process.h>
#include <d3dcommon.h>
#include <strsafe.h>
#include "LAssert.h"
#include "L3DEngine.h"

static const D3D_FEATURE_LEVEL FEATURE_LEVEL_ARRAY_0[] =
{
    D3D_FEATURE_LEVEL_11_0,
};

static const D3D_FEATURE_LEVEL FEATURE_LEVEL_ARRAY_1[] =
{
    D3D_FEATURE_LEVEL_11_1,
};

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

extern ID3D11Device* g_p3DDevice = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    L3DEngine* pEngine = dynamic_cast<L3DEngine*>(L3DEngine::Instance());
    return pEngine->MsgProc(hWnd, msg, wParam, lParam);
}

L3DEngine::L3DEngine()
: m_piSwapChain(nullptr)
, m_piRenderTargetView(nullptr)
, m_bActive(false)
, m_CurSampFilter(m_SampFilter[GRAPHICS_LEVEL_MAX])
{
    memset(&m_SampFilter, 0, sizeof(m_SampFilter));
    memset(&m_WindowParam, 0, sizeof(m_WindowParam));
    memset(&m_Viewport, 0, sizeof(m_Viewport));
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

    hr = InitL3DWindow(&hWnd, hInstance);
    HRESULT_ERROR_EXIT(hr);

    hr = InitSwapChain(m_Device.piDevice, m_WindowParam.Width, m_WindowParam.Height, hWnd);
    HRESULT_ERROR_EXIT(hr);

	hr = InitShaderTable(m_Device.piDevice);
	HRESULT_ERROR_EXIT(hr);

    hr = InitViewport(m_WindowParam.Width, m_WindowParam.Height);
    HRESULT_ERROR_EXIT(hr);

    hr = InitSamplerFilter();
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

    hr = UpdateMessage();
    HRESULT_ERROR_EXIT(hr);

    float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    m_Device.piImmediateContext->ClearRenderTargetView(m_piRenderTargetView, clearColor);

    hr = m_piSwapChain->Present(0, 0);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

BOOL L3DEngine::IsActive()
{
    return m_bActive;
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

HRESULT L3DEngine::InitSwapChain(ID3D11Device *piDevice, unsigned uWidth, unsigned uHeight, HWND hWnd)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    IDXGIDevice *piDXGIDevice = nullptr;
    IDXGIAdapter *piDXGIAdapter = nullptr;
    IDXGIFactory *piDXGIFactory = nullptr;
    DXGI_SWAP_CHAIN_DESC SwapChainDesc;

    // DXGI_MODE_DESC
    SwapChainDesc.BufferCount = 1;
    SwapChainDesc.BufferDesc.Width = uWidth;
    SwapChainDesc.BufferDesc.Height = uHeight;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    // DXGI_SAMPLE_DESC
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;

    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.OutputWindow = hWnd;
    SwapChainDesc.Windowed = TRUE; // from SDK: should not create a full-screen swap chain
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDesc.Flags = 0;

    hr = piDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&piDXGIDevice);
    HRESULT_ERROR_EXIT(hr);

    hr = piDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&piDXGIAdapter);
    HRESULT_ERROR_EXIT(hr);

    hr = piDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&piDXGIFactory);
    HRESULT_ERROR_EXIT(hr);

    hr = piDXGIFactory->CreateSwapChain(piDevice, &SwapChainDesc, &m_piSwapChain);
    HRESULT_ERROR_EXIT(hr);

    SAFE_RELEASE(piDXGIDevice);
    SAFE_RELEASE(piDXGIAdapter);
    SAFE_RELEASE(piDXGIFactory);

    hr = InitRenderTargetView(piDevice, m_piSwapChain);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DEngine::InitRenderTargetView(ID3D11Device *piDevice, IDXGISwapChain* piSwapChain)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    ID3D11Texture2D *piSwapChainBuffer = nullptr;

    BOOL_ERROR_EXIT(piDevice);
    BOOL_ERROR_EXIT(m_Device.piImmediateContext);

    hr = piSwapChain->GetBuffer(0, __uuidof(piSwapChainBuffer), reinterpret_cast<void**>(&piSwapChainBuffer));
    HRESULT_ERROR_EXIT(hr);

    hr = piDevice->CreateRenderTargetView(piSwapChainBuffer, nullptr, &m_piRenderTargetView);
    HRESULT_ERROR_EXIT(hr);

    m_Device.piImmediateContext->OMSetRenderTargets(1, &m_piRenderTargetView, nullptr);

    hResult = S_OK;
Exit0:
    SAFE_RELEASE(piSwapChainBuffer);
    if (FAILED(hResult))
    {
        SAFE_RELEASE(m_piRenderTargetView);
    }
    return hResult;
}


HRESULT L3DEngine::InitShaderTable(ID3D11Device* piDevice)
{
    HRESULT hResult = E_FAIL;
    
    m_pShaderTable = CreateShaderTable(piDevice);
    BOOL_ERROR_EXIT(m_pShaderTable);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DEngine::InitViewport(unsigned uWidth, unsigned uHeight)
{
    HRESULT hResult = E_FAIL;

    BOOL_ERROR_EXIT(m_Device.piImmediateContext);

    m_Viewport.TopLeftX = 0;
    m_Viewport.TopLeftY = 0;
    m_Viewport.Width = (FLOAT)uWidth;
    m_Viewport.Height = (FLOAT)uHeight;
    m_Viewport.MinDepth = 0.0f;
    m_Viewport.MaxDepth = 1.0f;

    m_Device.piImmediateContext->RSSetViewports(1, &m_Viewport);

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