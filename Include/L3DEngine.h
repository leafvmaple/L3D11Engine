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
class L3DLog;

class L3DEngine : public IL3DEngine
{
public:
    L3DEngine();
    virtual ~L3DEngine();

    virtual bool Init(HINSTANCE hInstance, L3D_WINDOW_PARAM& WindowParam);
    virtual void Uninit();
    
    virtual void Update(float fDeltaTime);

    virtual ID3D11Device* GetDevice() const;

    virtual DWORD GetFrame() const override;
    virtual float GetTime() const override;

public:
    bool AttachScene(L3DScene* pScene);

private:
    struct _DEVICE
    {
        D3D_FEATURE_LEVEL   eFeatureLevel = D3D_FEATURE_LEVEL_10_0;
        ID3D11Device        *piDevice = nullptr;
        ID3D11DeviceContext *piImmediateContext = nullptr;
    } m_Device;

    std::shared_ptr<L3D_SHADER_TABLE> m_pShaderTable = nullptr;
    std::shared_ptr<L3D_STATE_TABLE> m_pStateTable = nullptr;

    std::unique_ptr<L3DWindow> m_pWindow = nullptr;
    std::unique_ptr<L3DLog> m_pLog = nullptr;

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
    D3D11_SAMPLER_DESC& m_CurSampFilter = m_SampFilter[GRAPHICS_LEVEL_MAX];

    ID3D11SamplerState* m_pSamplerState[GRAPHICS_LEVEL_COUNT] = {0};

    L3D_WINDOW_PARAM m_WindowParam = { 0 };

private:

    bool _SetupD3D();
    bool _CreateTargetWindow(HWND hWnd);
    bool _CreateDeivice();
    bool _CreateSamplerFilter();
};