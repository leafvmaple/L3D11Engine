#pragma once

struct ID3DX11EffectTechnique;
struct ID3DX11EffectConstantBuffer;

struct MESH_SHARED_CB;

enum RUNTIME_MACRO
{
    RUNTIME_MACRO_MESH = 0,
    RUNTIME_MACRO_SKIN_MESH = 1,
};

class L3DMaterial
{
public:
    HRESULT Create(ID3D11Device* piDevice, RUNTIME_MACRO eMacro);
    HRESULT Apply(ID3D11DeviceContext* pDeviceContext);
    HRESULT SetData(MESH_SHARED_CB* pData);

    MESH_SHARED_CB* m_pData = nullptr;

private:
    ID3DX11EffectTechnique* m_piEffectTech = nullptr;
    ID3DX11EffectConstantBuffer* m_piModelSharedCB = nullptr;

};
