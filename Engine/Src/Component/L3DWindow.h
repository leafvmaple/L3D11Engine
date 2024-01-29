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
    bool Init(ID3D11Device* piDevice, HWND hWnd);
    void AddScene(L3DScene* pScene);

    void BeginPaint(ID3D11Device* piDevice, const SCENE_RENDER_OPTION& RenderOption);
    void EndPaint(ID3D11Device* piDevice, const SCENE_RENDER_OPTION& RenderOption);
    void Present();

    ~L3DWindow() {}

private:
    bool _CreateSwapChain(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight, HWND hWnd);
    bool _CreateViewport(unsigned uWidth, unsigned uHeight);

    bool _CreateStencilView(ID3D11Device* piDevice, unsigned uWidth, unsigned uHeight);
    bool _CreateSwapChainRTV(ID3D11Device* piDevice, IDXGISwapChain* piSwapChain);

    void _UpdateAll3DScene(const SCENE_RENDER_OPTION& RenderOption);

    std::vector<L3DScene*> m_SceneList;

    IDXGISwapChain* m_piSwapChain = nullptr;
    ID3D11DeviceContext* m_piImmediateContext = nullptr;

    ID3D11RenderTargetView* m_piRenderTargetView = nullptr;
    ID3D11DepthStencilView* m_piDepthStencilView = nullptr;

    D3D11_VIEWPORT m_Viewport = {0};
};

bool Create3DWindow(std::unique_ptr<L3DWindow>* pWindow, ID3D11Device* piDevice, HWND hWnd);