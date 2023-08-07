#pragma once

#include "L3DInterface.h"

#include <vector>
#include <memory>

class L3DScene;
class L3DModel;

struct SCENE_RENDER_OPTION;

class L3DWindow
{
public:
    HRESULT Init(ID3D11Device* piDevice, HWND hWnd);
    HRESULT AddScene(L3DScene* pScene);

    void BeginPaint(ID3D11Device* piDevice, const SCENE_RENDER_OPTION& RenderOption);
    void EndPaint(ID3D11Device* piDevice, const SCENE_RENDER_OPTION& RenderOption);
    void Present();

    ~L3DWindow() {}

private:
    HRESULT _CreateSwapChain(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight, HWND hWnd);
    HRESULT _CreateViewport(unsigned uWidth, unsigned uHeight);

    HRESULT _CreateStencilView(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight);
    HRESULT _CreateSwapChainRTV(ID3D11Device* piDevice, IDXGISwapChain* piSwapChain);

    std::vector<L3DScene*> m_SceneList;

    IDXGISwapChain* m_piSwapChain = nullptr;
    ID3D11DeviceContext* m_piImmediateContext = nullptr;

    ID3D11RenderTargetView* m_piRenderTargetView = nullptr;
    ID3D11DepthStencilView* m_piDepthStencilView = nullptr;

    D3D11_VIEWPORT m_Viewport;
};

std::shared_ptr<L3DWindow> L3DCreateWindow(ID3D11Device* piDevice, HWND hWnd);