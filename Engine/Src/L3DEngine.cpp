#include <Windows.h>
#include <process.h>
#include <d3dcommon.h>
#include <strsafe.h>
#include <d3d11.h>

#include "LAssert.h"
#include "L3DEngine.h"

#include "Component/L3DShader.h"
#include "Component/L3DMesh.h"
#include "Component/L3DEffect.h"

#include "Input/L3DInput.h"
#include "Camera/L3DCamera.h"

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
: m_piSwapChain(nullptr)
, m_piRenderTargetView(nullptr)
, m_piDepthStencilView(nullptr)
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

    hr = UpdateMessage();
    HRESULT_ERROR_EXIT(hr);

    hr = UpdateInput();
    HRESULT_ERROR_EXIT(hr);

    hr = UpdateCamera(fDeltaTime);
    HRESULT_ERROR_EXIT(hr);
    
    m_Device.piImmediateContext->ClearRenderTargetView(m_piRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
    m_Device.piImmediateContext->ClearDepthStencilView(m_piDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    for (const auto& stage : m_IAStage)
    {
        hr = DrawEffect(stage);
        HRESULT_ERROR_EXIT(hr);
    }

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


ID3D11Device* L3DEngine::GetDevice() const
{
    return m_Device.piDevice;
}


HRESULT L3DEngine::AttachObject(L3DRenderUnit* pUnit)
{
    pUnit->PushRenderUnit(m_IAStage);

    return S_OK;
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

    // TODO
    hr = InitStencilView(piDevice, uWidth, uHeight);
    HRESULT_ERROR_EXIT(hr);

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

    m_Device.piImmediateContext->OMSetRenderTargets(1, &m_piRenderTargetView, m_piDepthStencilView);

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

    m_pEffectTable = CreateEffectTable(piDevice);
    BOOL_ERROR_EXIT(m_pEffectTable);

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

HRESULT L3DEngine::DrawEffect(const RENDER_STAGE_INPUT_ASSEMBLER& stage)
{
    L3D_EFFECT_TABLE::_EFFECT_INFO* pEffectInfo = nullptr;
    D3DX11_TECHNIQUE_DESC techDesc;

    XMMATRIX worldViewProj = m_pCamera->GetWorldViewProjcetion(XMMatrixIdentity());

    pEffectInfo = &m_pEffectTable->Table[stage.eShaderEffect];

    m_Device.piImmediateContext->IASetInputLayout(pEffectInfo->pLayout);
    m_Device.piImmediateContext->IASetPrimitiveTopology(stage.eTopology);

    m_Device.piImmediateContext->IASetVertexBuffers(0, 1, &stage.VertexBuffer.piBuffer, &stage.VertexBuffer.uStride, &stage.VertexBuffer.uOffset);
    m_Device.piImmediateContext->IASetIndexBuffer(stage.IndexBuffer.piBuffer, stage.IndexBuffer.eFormat, stage.IndexBuffer.uOffset);

    pEffectInfo->pWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
    pEffectInfo->pEffectTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        pEffectInfo->pEffectTech->GetPassByIndex(p)->Apply(0, m_Device.piImmediateContext);
    }

    m_Device.piImmediateContext->DrawIndexed(stage.Draw.Indexed.uIndexCount, stage.Draw.Indexed.uStartIndexLocation, stage.Draw.Indexed.nBaseVertexLocation);

    return S_OK;
}

HRESULT L3DEngine::InitStencilView(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    ID3D11Texture2D *pDepthStencilBuffer = nullptr;
    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width = uWidth;
    depthStencilDesc.Height = uHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;

    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    hr = piDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer);
    HRESULT_ERROR_EXIT(hr);

    hr = piDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &m_piDepthStencilView);
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
