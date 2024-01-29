#include "stdafx.h"

#include "L3DWindow.h"
#include "Camera/L3DCamera.h"
#include "Scene/L3DScene.h"

#include "L3DFormat.h"
#include "LMemory.h"

bool L3DWindow::Init(ID3D11Device* piDevice, HWND hWnd)
{
    LONG nWidth = 0;
    LONG nHeight = 0;
    RECT Rect;

    piDevice->GetImmediateContext(&m_piImmediateContext);
    ::GetClientRect(hWnd, &Rect);

    nWidth = Rect.right - Rect.left;
    nHeight = Rect.bottom - Rect.top;

    CHECK_BOOL(_CreateSwapChain(piDevice, nWidth, nHeight, hWnd));
    CHECK_BOOL(_CreateViewport(nWidth, nHeight));
    
    return true;
}

void L3DWindow::AddScene(L3DScene* pScene)
{
    m_SceneList.emplace_back(pScene);
}

void L3DWindow::BeginPaint(ID3D11Device* piDevice, const SCENE_RENDER_OPTION& RenderOption)
{
    for (auto& scene : m_SceneList)
    {
        CAMERA_PROPERTY Property;
        L3DCamera* pCamera = scene->GetCamera();

        pCamera->GetProperty(Property);
        Property.Persective.fFovAngleY = XM_PIDIV2;
        Property.Persective.fAspectRatio = m_Viewport.Width / m_Viewport.Height;

        pCamera->SetProperty(Property);

        scene->UpdateCamera();
    }

    _UpdateAll3DScene(RenderOption);

    for (auto& pScene : m_SceneList)
        pScene->BeginRender(RenderOption);
}

void L3DWindow::EndPaint(ID3D11Device* piDevice, const SCENE_RENDER_OPTION& RenderOption)
{
    m_piImmediateContext->ClearRenderTargetView(m_piRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
    m_piImmediateContext->ClearDepthStencilView(m_piDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    for (auto& pScene : m_SceneList)
        pScene->EndRender(RenderOption);
}

void L3DWindow::Present()
{
    m_piSwapChain->Present(0, 0);
}

bool L3DWindow::_CreateSwapChain(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight, HWND hWnd)
{
    auto piDXGIDevice = L3D::make_resource<IDXGIDevice>();
    auto piDXGIAdapter = L3D::make_resource<IDXGIAdapter>();
    auto piDXGIFactory = L3D::make_resource<IDXGIFactory>();

    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {0};

    // DXGI_MODE_DESC
    SwapChainDesc.BufferCount                        = 1;
    SwapChainDesc.BufferDesc.Width                   = uWidth;
    SwapChainDesc.BufferDesc.Height                  = uHeight;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    SwapChainDesc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    // DXGI_SAMPLE_DESC
    SwapChainDesc.SampleDesc.Count                   = 1;     // 1X MASS (MultiSample Anti-Aliasing) 

    SwapChainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.OutputWindow                       = hWnd;
    SwapChainDesc.Windowed                           = true; // from SDK: should not create a full-screen swap chain
    SwapChainDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

    CHECK_HRESULT(piDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&piDXGIDevice));
    CHECK_HRESULT(piDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&piDXGIAdapter));
    CHECK_HRESULT(piDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&piDXGIFactory));
    CHECK_HRESULT(piDXGIFactory->CreateSwapChain(piDevice, &SwapChainDesc, &m_piSwapChain));

    // TODO
    CHECK_BOOL(_CreateStencilView(piDevice, uWidth, uHeight));
    CHECK_BOOL(_CreateSwapChainRTV(piDevice, m_piSwapChain));

    return true;
}

bool L3DWindow::_CreateViewport(unsigned uWidth, unsigned uHeight)
{
    CHECK_BOOL(m_piImmediateContext);

    m_Viewport.Width    = (FLOAT)uWidth;
    m_Viewport.Height   = (FLOAT)uHeight;
    m_Viewport.MinDepth = 0.0f;
    m_Viewport.MaxDepth = 1.0f;

    m_piImmediateContext->RSSetViewports(1, &m_Viewport);

    return true;
}

bool L3DWindow::_CreateStencilView(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight)
{
    ID3D11Texture2D* pDepthStencilBuffer = nullptr;
    D3D11_TEXTURE2D_DESC depthStencilDesc = {0};

    depthStencilDesc.Width              = uWidth;
    depthStencilDesc.Height             = uHeight;
    depthStencilDesc.MipLevels          = 1;
    depthStencilDesc.ArraySize          = 1;
    depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;

    depthStencilDesc.SampleDesc.Count   = 1;

    depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;

    CHECK_HRESULT(piDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer));
    CHECK_HRESULT(piDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &m_piDepthStencilView));

    return true;
}

bool L3DWindow::_CreateSwapChainRTV(ID3D11Device* piDevice, IDXGISwapChain* piSwapChain)
{
    auto piSwapChainBuffer = L3D::make_resource<ID3D11Texture2D>();

    CHECK_BOOL(piDevice);
    CHECK_BOOL(m_piImmediateContext);

    // TODO m_piRenderTargetView memory leak
    CHECK_HRESULT(piSwapChain->GetBuffer(0, __uuidof(piSwapChainBuffer), reinterpret_cast<void**>(&piSwapChainBuffer)));
    CHECK_HRESULT(piDevice->CreateRenderTargetView(piSwapChainBuffer.get(), nullptr, &m_piRenderTargetView));

    m_piImmediateContext->OMSetRenderTargets(1, &m_piRenderTargetView, m_piDepthStencilView);

    return true;
}

void L3DWindow::_UpdateAll3DScene(const SCENE_RENDER_OPTION& RenderOption)
{
    for (auto& scene : m_SceneList)
        scene->Update(RenderOption);
}

bool Create3DWindow(std::unique_ptr<L3DWindow>* pWindow, ID3D11Device* piDevice, HWND hWnd)
{
    auto pointer = std::make_unique<L3DWindow>();

    CHECK_BOOL(pointer->Init(piDevice, hWnd));
    
    *pWindow = std::move(pointer);
    return true;
}
