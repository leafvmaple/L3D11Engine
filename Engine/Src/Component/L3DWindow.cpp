#include "stdafx.h"

#include "L3DWindow.h"
#include "Scene/L3DScene.h"

#include "L3DFormat.h"

HRESULT L3DWindow::Init(ID3D11Device* piDevice, HWND hWnd)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	RECT Rect;
	LONG nWidth = 0;
	LONG nHeight = 0;

	piDevice->GetImmediateContext(&m_piImmediateContext);
	BOOL_ERROR_EXIT(m_piImmediateContext);

	::GetClientRect(hWnd, &Rect);

	nWidth = Rect.right - Rect.left;
	nHeight = Rect.bottom - Rect.top;

	hr = _CreateSwapChain(piDevice, nWidth, nHeight, hWnd);
	HRESULT_ERROR_EXIT(hr);

	hr = _CreateViewport(nWidth, nHeight);
	HRESULT_ERROR_EXIT(hr);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT L3DWindow::Update(ID3D11Device* piDevice, const SCENE_RENDER_OPTION& RenderOption)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	m_piImmediateContext->ClearRenderTargetView(m_piRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	m_piImmediateContext->ClearDepthStencilView(m_piDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	hr = m_SceneList[0]->Update(RenderOption);
	HRESULT_ERROR_EXIT(hr);

	hr = m_piSwapChain->Present(0, 0);
	HRESULT_ERROR_EXIT(hr);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT L3DWindow::AddScene(L3DScene* pScene)
{
	m_SceneList.push_back(pScene);

	return S_OK;
}

HRESULT L3DWindow::_CreateSwapChain(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight, HWND hWnd)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	IDXGIDevice* piDXGIDevice = nullptr;
	IDXGIAdapter* piDXGIAdapter = nullptr;
	IDXGIFactory* piDXGIFactory = nullptr;
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
	hr = _CreateStencilView(piDevice, uWidth, uHeight);
	HRESULT_ERROR_EXIT(hr);

	hr = _CreateSwapChainRTV(piDevice, m_piSwapChain);
	HRESULT_ERROR_EXIT(hr);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT L3DWindow::_CreateViewport(unsigned uWidth, unsigned uHeight)
{
	HRESULT hResult = E_FAIL;

	BOOL_ERROR_EXIT(m_piImmediateContext);

	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = (FLOAT)uWidth;
	m_Viewport.Height = (FLOAT)uHeight;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_piImmediateContext->RSSetViewports(1, &m_Viewport);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT L3DWindow::_CreateStencilView(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	ID3D11Texture2D* pDepthStencilBuffer = nullptr;
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

HRESULT L3DWindow::_CreateSwapChainRTV(ID3D11Device* piDevice, IDXGISwapChain* piSwapChain)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	ID3D11Texture2D* piSwapChainBuffer = nullptr;

	BOOL_ERROR_EXIT(piDevice);
	BOOL_ERROR_EXIT(m_piImmediateContext);

	hr = piSwapChain->GetBuffer(0, __uuidof(piSwapChainBuffer), reinterpret_cast<void**>(&piSwapChainBuffer));
	HRESULT_ERROR_EXIT(hr);

	hr = piDevice->CreateRenderTargetView(piSwapChainBuffer, nullptr, &m_piRenderTargetView);
	HRESULT_ERROR_EXIT(hr);

	m_piImmediateContext->OMSetRenderTargets(1, &m_piRenderTargetView, m_piDepthStencilView);

	hResult = S_OK;
Exit0:
	SAFE_RELEASE(piSwapChainBuffer);
	if (FAILED(hResult))
	{
		SAFE_RELEASE(m_piRenderTargetView);
	}
	return hResult;
}

L3DWindow* L3DCreateWindow(ID3D11Device* piDevice, HWND hWnd)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	L3DWindow* pWindow = new L3DWindow;
	BOOL_ERROR_EXIT(pWindow);

	hr = pWindow->Init(piDevice, hWnd);
	HRESULT_ERROR_EXIT(hr);

	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
		SAFE_DELETE(pWindow);
	return pWindow;
}