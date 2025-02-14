#include "stdafx.h"
#include "L3DMaterialSystem.h"

#include "Model/L3DMaterialConfig.h"

static unsigned s_CommonCBSlot = 13;
static unsigned s_CommonCSSlot = 15;

int L3DMaterialSystem::Init(ID3D11Device* piDevice)
{
    m_piDevice = piDevice;

    CHECK_BOOL(_InitCommonConstBuffer());

    return true;
}

void L3DMaterialSystem::SetCommonShaderData(ID3D11DeviceContext* piDeviceContext, const SHARED_SHADER_COMMON& CommonParam)
{
    D3D11_MAPPED_SUBRESOURCE MappedResource;

    piDeviceContext->Map(m_piCommonCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    memcpy(MappedResource.pData, &CommonParam, sizeof(SHARED_SHADER_COMMON));

    piDeviceContext->Unmap(m_piCommonCB, 0);

    // Why
#if 0
    piDeviceContext->VSSetConstantBuffers(s_CommonCBSlot, 1, &m_piCommonCB);
    piDeviceContext->HSSetConstantBuffers(s_CommonCBSlot, 1, &m_piCommonCB);
    piDeviceContext->DSSetConstantBuffers(s_CommonCBSlot, 1, &m_piCommonCB);
    piDeviceContext->PSSetConstantBuffers(s_CommonCBSlot, 1, &m_piCommonCB);
    piDeviceContext->GSSetConstantBuffers(s_CommonCBSlot, 1, &m_piCommonCB);
    piDeviceContext->CSSetConstantBuffers(s_CommonCBSlot, 1, &m_piCommonCB);
#else
    m_Buffers[s_CommonCBSlot] = m_piCommonCB;
#endif

}

void L3DMaterialSystem::SetCommonShaderSamples(ID3D11DeviceContext* piDeviceContext, ID3D11SamplerState* const* pSamplers)
{
    piDeviceContext->VSSetSamplers(s_CommonCSSlot, MTLSYS_COMMON_SPL_COUNT, pSamplers);
    piDeviceContext->PSSetSamplers(s_CommonCSSlot, MTLSYS_COMMON_SPL_COUNT, pSamplers);
}

bool L3DMaterialSystem::_InitCommonConstBuffer()
{
    D3D11_BUFFER_DESC bufDesc = {0};

    bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufDesc.ByteWidth = sizeof(SHARED_SHADER_COMMON);
    bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufDesc.Usage = D3D11_USAGE_DYNAMIC;

    CHECK_HRESULT(m_piDevice->CreateBuffer(&bufDesc, nullptr, &m_piCommonCB));

    return true;
}

L3DMaterialSystem* g_pMaterialSystem = new L3DMaterialSystem;