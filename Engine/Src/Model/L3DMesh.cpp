#include "stdafx.h"

#include <algorithm>
#include <map>

#include "L3DMesh.h"

#include "L3DBone.h"
#include "L3DMaterial.h"
#include "IO/LFileReader.h"
#include "IMesh.h"

bool L3DMesh::Create(ID3D11Device* piDevice, const char* szFileName)
{
    MESH_DESC desc = { szFileName };
    MESH_SOURCE source;

    LoadMesh(&desc, &source);

    CHECK_BOOL(_CreateMesh(piDevice, &source));
    CHECK_BOOL(_CreateBone(&source));

    m_sName = szFileName;

    return true;
}


void L3DMesh::ApplyMeshSubset(RENDER_STAGE_INPUT_ASSEMBLER& State, unsigned int nSubsetIndex)
{
    auto& subset = m_Subset[nSubsetIndex];

    State.nTopology    = m_nTopology;
    State.nInputLayout = m_nInputLayout;

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

bool L3DMesh::_CreateMesh(ID3D11Device* piDevice, const MESH_SOURCE* pSource)
{
    CHECK_BOOL(_CreateVertexBuffer(piDevice, pSource));
    CHECK_BOOL(_CreateIndexBuffer<WORD>(piDevice, pSource, DXGI_FORMAT_R16_UINT));

    m_nTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    assert(pSource->nVertexFVF & FVF_TANGENT);
    assert(pSource->nVertexFVF & FVF_TEX1);

    if (pSource->nVertexFVF & FVF_SKIN)
        m_nInputLayout = L3D_INPUT_LAYOUT_CI_SKINMESH;
    else
        m_nInputLayout = L3D_INPUT_LAYOUT_CI_MESH;

    return true;
}

bool L3DMesh::_CreateVertexBuffer(ID3D11Device* piDevice, const MESH_SOURCE* pSource)
{
    D3D11_BUFFER_DESC desc = {0};
    D3D11_SUBRESOURCE_DATA InitData = {0};

    m_nVertexSize = pSource->nVertexSize;

    desc.ByteWidth   = m_nVertexSize * pSource->nVerticesCount;;
    desc.Usage       = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags   = D3D11_BIND_VERTEX_BUFFER;

    InitData.pSysMem = pSource->pVertices;

    CHECK_HRESULT(piDevice->CreateBuffer(&desc, &InitData, &m_piVertexBuffer));

    return true;
}

template<typename T>
bool L3DMesh::_CreateIndexBuffer(ID3D11Device* piDevice, const MESH_SOURCE* pSource, DXGI_FORMAT nFormat)
{
    D3D11_BUFFER_DESC desc = {0};
    D3D11_SUBRESOURCE_DATA InitData = {0};
    std::vector<T> Indices;
    UINT nOffset = 0;

    for (int i = 0; i < pSource->nSubsetCount; i++)
    {
        m_Subset.emplace_back(_SUBSET{ pSource->pSubsetVertexCount[i], nOffset });
        nOffset += pSource->pSubsetVertexCount[i];
    }

    for (int i = 0; i < pSource->nIndexCount; i++)
        Indices.emplace_back(static_cast<T>(pSource->pIndices[i]));

    m_eFormat = nFormat;

    desc.ByteWidth   = sizeof(T) * Indices.size();
    desc.Usage       = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags   = D3D11_BIND_INDEX_BUFFER;

    InitData.pSysMem = Indices.data();

    CHECK_HRESULT(piDevice->CreateBuffer(&desc, &InitData, &m_piIndexBuffer));

    return true;
}

bool L3DMesh::_CreateBone(const MESH_SOURCE* pSource)
{
    m_pL3DBone = new(std::nothrow) L3DBone;
    CHECK_BOOL(m_pL3DBone);
    CHECK_BOOL(m_pL3DBone->Create(pSource));

    m_dwBoneCount = pSource->nBonesCount;

    m_BoneMatrix.reserve(m_dwBoneCount);
    for (const auto& M : m_pL3DBone->m_BoneOffset)
        m_BoneMatrix.emplace_back(XMMatrixInverse(nullptr, M));

    return true;
}
