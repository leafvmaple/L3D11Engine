#pragma once

#include <d3d11.h>
#include <unordered_map>

class L3DEffect;
struct SHARED_SHADER_COMMON;

typedef std::unordered_map<int, ID3D11Buffer*> COMMON_CB_MAP;

class L3DMaterialSystem
{
public:
    void Init(ID3D11Device* piDevice);

    void SetCommonShaderData(ID3D11DeviceContext* piDeviceContext, const SHARED_SHADER_COMMON& CommonParam);
    COMMON_CB_MAP& GetCommonCBList() { return m_Buffers; };

private:
    void _InitCommonConstBuffer();

    ID3D11Device* m_piDevice = nullptr;
    ID3D11Buffer* m_piCommonCB = nullptr;

    COMMON_CB_MAP m_Buffers;

};

extern L3DMaterialSystem* g_pMaterialSystem;