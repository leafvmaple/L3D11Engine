#pragma once

#include <d3d11.h>
#include <unordered_map>

class L3DEffect;
struct SHARED_SHADER_COMMON;
struct SHARED_SHADER_SAMPLERS;

enum MTLSYS_COMMON_SPL
{
    MTLSYS_COMMON_SPL_POINTCLAMP,          //SamplerState g_sPointClamp              : register(s15);
    MTLSYS_COMMON_SPL_COUNT,
};

typedef std::unordered_map<int, ID3D11Buffer*> COMMON_CB_MAP;

class L3DMaterialSystem
{
public:
    int Init(ID3D11Device* piDevice);

    void SetCommonShaderData(ID3D11DeviceContext* piDeviceContext, const SHARED_SHADER_COMMON& CommonParam);
    void SetCommonShaderSamples(ID3D11DeviceContext* piDeviceContext, ID3D11SamplerState* const* pSamplers);

    COMMON_CB_MAP& GetCommonCBList() { return m_Buffers; };

private:
    bool _InitCommonConstBuffer();

    ID3D11Device* m_piDevice = nullptr;
    ID3D11Buffer* m_piCommonCB = nullptr;

    COMMON_CB_MAP m_Buffers;

};

extern L3DMaterialSystem* g_pMaterialSystem;