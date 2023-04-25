#pragma once

#include <d3d11.h>
#include <vector>
#include <list>

#include "L3DInterface.h"
#include "Render/L3DRenderUnit.h"

struct L3D_SHADER_TABLE;
struct L3D_STATE_TABLE;

class L3DWindow;
class L3DInput;
class L3DModel;
class L3DScene;

class L3DEngine : public IL3DEngine
{
    friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    L3DEngine();
    virtual ~L3DEngine();

    virtual HRESULT Init(HINSTANCE hInstance, L3D_WINDOW_PARAM& WindowParam);
    virtual HRESULT Uninit();
    
    virtual HRESULT Update(float fDeltaTime);

    virtual BOOL IsActive();
    virtual ID3D11Device* GetDevice() const;

public:
    HRESULT AttachScene(L3DScene* pScene);

private:
    struct _DEVICE
    {
        D3D_FEATURE_LEVEL    eFeatureLevel;
        ID3D11Device        *piDevice;
        ID3D11DeviceContext *piImmediateContext;
    } m_Device;

    L3D_SHADER_TABLE* m_pShaderTable = nullptr;
    L3D_STATE_TABLE* m_pStateTable = nullptr;

    L3DWindow* m_pWindow = nullptr;
    L3DInput* m_pInput = nullptr;

    L3D_WINDOW_PARAM m_WindowParam;

    D3D11_SAMPLER_DESC m_SampFilter[GRAPHICS_LEVEL_COUNT] = {
        {
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,   //D3D11_FILTER Filter;
            D3D11_TEXTURE_ADDRESS_WRAP, //D3D11_TEXTURE_ADDRESS_MODE AddressU;
            D3D11_TEXTURE_ADDRESS_WRAP, //D3D11_TEXTURE_ADDRESS_MODE AddressV;
            D3D11_TEXTURE_ADDRESS_WRAP, //D3D11_TEXTURE_ADDRESS_MODE AddressW;
            0,                          //FLOAT MipLODBias;
            8,                          //UINT MaxAnisotropy;
            D3D11_COMPARISON_NEVER,     //D3D11_COMPARISON_FUNC ComparisonFunc;
            {0, 0, 0, 0},               //FLOAT BorderColor[ 4 ];
            0.0f,                       //FLOAT MinLOD;
            D3D11_FLOAT32_MAX           //FLOAT MaxLOD;
        },
        {
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,   //D3D11_FILTER Filter;
            D3D11_TEXTURE_ADDRESS_WRAP, //D3D11_TEXTURE_ADDRESS_MODE AddressU;
            D3D11_TEXTURE_ADDRESS_WRAP, //D3D11_TEXTURE_ADDRESS_MODE AddressV;
            D3D11_TEXTURE_ADDRESS_WRAP, //D3D11_TEXTURE_ADDRESS_MODE AddressW;
            0,                          //FLOAT MipLODBias;
            8,                          //UINT MaxAnisotropy;
            D3D11_COMPARISON_NEVER,     //D3D11_COMPARISON_FUNC ComparisonFunc;
            {0, 0, 0, 0},               //FLOAT BorderColor[ 4 ];
            0.0f,                       //FLOAT MinLOD;
            D3D11_FLOAT32_MAX           //FLOAT MaxLOD;
        }
    };
    D3D11_SAMPLER_DESC& m_CurSampFilter;

    ID3D11SamplerState* m_pSamplerState[GRAPHICS_LEVEL_COUNT];

    BOOL m_bActive;

private:
    LRESULT	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HRESULT InitL3DWindow(HWND* pWnd, HINSTANCE hInstance);
    HRESULT InitSamplerFilter();
    HRESULT InitInput(HWND hWnd, HINSTANCE hInstance);

    HRESULT _SetupD3D();
    HRESULT _CreateTargetWindow(HWND hWnd);
    HRESULT _CreateDeivice();

    HRESULT UpdateMessage();
    HRESULT UpdateInput();
    HRESULT UpdateCamera(float fDeltaTime);
};