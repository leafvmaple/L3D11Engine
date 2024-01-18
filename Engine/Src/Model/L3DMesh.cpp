#include "stdafx.h"

#include <algorithm>
#include <map>

#include "L3DMesh.h"

#include "L3DBone.h"
#include "L3DMaterial.h"
#include "IO/LFileReader.h"
#include "IMesh.h"

HRESULT L3DMesh::Create(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    MESH_DESC desc = { szFileName };
    MESH_SOURCE* pSource;

    LoadMesh(&desc, pSource);

    hr = CreateMesh(piDevice, pSource);
    HRESULT_ERROR_EXIT(hr);

    pSource->Release();

    m_sName = szFileName;

    hResult = S_OK;
Exit0:
    return hResult;
}


void L3DMesh::ApplyMeshSubset(RENDER_STAGE_INPUT_ASSEMBLER& State, unsigned int nSubsetIndex)
{
    auto& subset = m_Subset[nSubsetIndex];

    State.eTopology    = m_eTopology;
    State.eInputLayout = m_eInputLayout;

    State.VertexBuffer.piBuffer = m_piVertexBuffer;
    State.VertexBuffer.uOffset  = 0;
    State.VertexBuffer.uStride  = m_nVertexSize;

    State.IndexBuffer.piBuffer = m_piIndexBuffer;
    State.IndexBuffer.uOffset  = 0;
    State.IndexBuffer.eFormat  = m_eFormat;

    State.Draw.Indexed.uIndexCount         = subset.uIndexCount;
    State.Draw.Indexed.uStartIndexLocation = subset.uIndexOffset;
    State.Draw.Indexed.nBaseVertexLocation = 0;
}

HRESULT L3DMesh::CreateMesh(ID3D11Device* piDevice, const MESH_SOURCE* pSource)
{
    InitVertexBuffer(piDevice, pSource);
    InitIndexBuffer<WORD>(piDevice, pSource, DXGI_FORMAT_R16_UINT);

    _CreateBone(pSource);

    m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    m_nVertexSize = pSource->nVertexSize;

    assert(pSource->nVertexFVF & FVF_TANGENT);
    if (pSource->nVertexFVF & FVF_TEX1)
    {
        if (pSource->nVertexFVF & FVF_SKIN)
            m_eInputLayout = L3D_INPUT_LAYOUT_CI_SKINMESH;
        else
            m_eInputLayout = L3D_INPUT_LAYOUT_CI_MESH;
    }
    else
        assert(false);

Exit0:
    return S_OK;
}

HRESULT L3DMesh::InitVertexBuffer(ID3D11Device* piDevice, const MESH_SOURCE* pSource)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    DWORD uBufferSize = 0;

    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA InitData;

    uBufferSize = pSource->nVertexSize * pSource->nVerticesCount;

    desc.ByteWidth           = uBufferSize;
    desc.Usage               = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags      = 0;
    desc.MiscFlags           = 0;
    desc.StructureByteStride = 0;

    InitData.pSysMem          = pSource->pVertices;
    InitData.SysMemPitch      = 0;
    InitData.SysMemSlicePitch = 0;

    hr = piDevice->CreateBuffer(&desc, &InitData, &m_piVertexBuffer);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

template<typename _INDEX_TYPE>
HRESULT L3DMesh::InitIndexBuffer(ID3D11Device* piDevice, const MESH_SOURCE* pSource, DXGI_FORMAT eFormat)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA InitData;
    std::vector<_INDEX_TYPE> Indices;

    unsigned int nOffset = 0;
    for (int i = 0; i < pSource->nSubsetCount; i++)
    {
        m_Subset.emplace_back(_SUBSET{ pSource->pSubsetVertexCount[i], nOffset });
        nOffset += pSource->pSubsetVertexCount[i];
    }

    for (int i = 0; i < pSource->nIndexCount; i++)
        Indices.emplace_back(static_cast<_INDEX_TYPE>(pSource->pIndices[i]));

    desc.ByteWidth           = sizeof(_INDEX_TYPE) * Indices.size();
    desc.Usage               = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags      = 0;
    desc.MiscFlags           = 0;
    desc.StructureByteStride = 0;

    InitData.pSysMem          = Indices.data();
    InitData.SysMemPitch      = 0;
    InitData.SysMemSlicePitch = 0;

    hr = piDevice->CreateBuffer(&desc, &InitData, &m_piIndexBuffer);
    HRESULT_ERROR_EXIT(hr);

    m_eFormat = eFormat;

    hResult = S_OK;
Exit0:
    return hResult;
}

void L3DMesh::_CreateBone(const MESH_SOURCE* pSource)
{
    m_pL3DBone = new(std::nothrow) L3DBone;
    m_pL3DBone->BindData(pSource);

    m_dwBoneCount = pSource->nBonesCount;

    m_BoneMatrix.resize(m_dwBoneCount);
    for (unsigned int i = 0; i < m_dwBoneCount; i++)
        m_BoneMatrix[i] = XMMatrixInverse(nullptr, m_pL3DBone->m_BoneOffset[i]);
}
