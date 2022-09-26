#pragma once

struct ID3DX11EffectTechnique;
struct ID3DX11EffectConstantBuffer;
struct ID3DX11EffectVariable;

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

    HRESULT SetVariableValue(MESH_SHARED_CB* pData);

    MESH_SHARED_CB* m_pData = nullptr;

private:
	struct MODEL_FIX_VARIBLES
	{
        ID3DX11EffectVariable* pModelParams;
	};

    ID3DX11EffectTechnique* m_piEffectTech = nullptr;
    ID3DX11EffectConstantBuffer* m_piModelSharedCB = nullptr;

    MODEL_FIX_VARIBLES m_Variables;
};
