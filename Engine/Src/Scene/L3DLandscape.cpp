#include "stdafx.h"
#include "L3DLandscape.h"

#include "L3DFormat.h"
#include "ILandscape.h"

#include "Model/L3DShader.h"
#include "Model/L3DMaterial.h"
#include "Render/L3DRenderUnit.h"

#include "FX11/inc/d3dx11effect.h"

struct INSTANCE_DATA
{
    XMINT4 NodeOffset;
    XMFLOAT4 NeighborLOD;
};

struct RENDER_REGION_DESC
{
    const MTLSYS_TERRAIN_CB& TerrainCB;
    RENDER_PASS RenderPass;
    UINT nIndies;
    ID3D11Buffer* pInstanceBuffer;
};

static const unsigned g_nTerrainSlotsBegin = 1;

HRESULT L3DLandscapeTerrainNode::Load(ID3D11Device* piDevice, const LANDSCAPE_SOURCE& source)
{
    m_Origin = { 0, 0 };
    _CreateChildNodes(source);

    return S_OK;
}

void L3DLandscapeTerrainNode::GetVisibleNodes(std::vector<L3DLandscapeTerrainNode*>& VisibleNodes)
{
    for (const auto& child : m_Children)
        VisibleNodes.emplace_back(child);
}

void L3DLandscapeTerrainNode::_CreateChildNodes(const LANDSCAPE_SOURCE& source)
{
    // TODO
    int nRowCount = source.RegionSize / source.LeafNodeSize;

    for (int i = 0; i < nRowCount; i++)
        for (int j = 0; j < nRowCount; j++)
        {
            L3DLandscapeTerrainNode* pChild = m_Children.emplace_back(new L3DLandscapeTerrainNode);
            pChild->m_Origin = { m_Origin.x + i * (int)source.LeafNodeSize, m_Origin.y + j * (int)source.LeafNodeSize };
        }
}

HRESULT L3DLandscapeRegion::Load(ID3D11Device* piDevice, const LANDSCAPE_REGION& region, const LANDSCAPE_SOURCE& source, const std::vector<L3DMaterial*>& materialPack)
{
    ID3DX11EffectConstantBuffer* piSharedCB = nullptr;

    m_pHeightMap = new L3DTexture;
    m_pHeightMap->Create(piDevice, region.pHeightData, region.nHeightData, region.nHeightData);

    m_Material.resize(region.nMaterial);
    for (int k = 0; k < region.nMaterial; k++)
        m_Material[k] = materialPack[region.pMaterialIDs[k]];

    m_nSize = source.RegionSize;

    // TODO: Always use the first material
    m_pMaterial = m_Material[0];
    m_pMaterial->CreateIndividualCB(MATERIAL_INDIV_CB::MODELSHARED, &piSharedCB);

    m_pModelParams = piSharedCB->GetMemberByName("g_ModelParams");

    m_pTerrainNode = new L3DLandscapeTerrainNode;
    m_pTerrainNode->m_Origin = m_Origin;
    m_pTerrainNode->Load(piDevice, source);

    return S_OK;
}

void L3DLandscapeRegion::UpdateVisiblity()
{
    m_TerrainVisibleNodes.clear();
    m_pTerrainNode->GetVisibleNodes(m_TerrainVisibleNodes);
}

void L3DLandscapeRegion::RenderTerrain(const SCENE_RENDER_OPTION& RenderOption, const RENDER_REGION_DESC& desc)
{
    MTLSYS_TERRAIN_CB TerrainCB = desc.TerrainCB;

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    UINT nStride = sizeof(INSTANCE_DATA);
    UINT nOffset = 0;

    // UINT nNumViews = sizeof(MTLSYS_TERRAIN_TEXTURES) / sizeof(ID3D11ShaderResourceView*);
    // ID3D11ShaderResourceView** ppSRVs = (ID3D11ShaderResourceView**)&TerrainTextures;

    // RenderOption.piImmediateContext->VSSetShaderResources(g_nTerrainSlotsBegin, _countof(piSRVs), piSRVs);
    // RenderOption.piImmediateContext->PSSetShaderResources(g_nTerrainSlotsBegin, _countof(piSRVs), piSRVs);

    TerrainCB.RegionOffset = m_Origin;
    TerrainCB.HeightMapUVScale = (1.0f - 1.0f / (float)m_nSize) / (float)m_nSize;
    TerrainCB.HeightMapUVBias = 0.5f / (float)m_nSize;

    m_pModelParams->SetRawValue(&TerrainCB, 0, sizeof(MTLSYS_TERRAIN_CB));

    m_pMaterial->SetTexture("g_TerrainHeightMap", m_pHeightMap);
    m_pMaterial->Apply(RenderOption.piImmediateContext, RENDER_PASS::COLOR);

    RenderOption.piImmediateContext->Map(desc.pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    INSTANCE_DATA* pInstanceData = (INSTANCE_DATA*)MappedResource.pData;

    for (int i = 0; i < m_TerrainVisibleNodes.size(); i++)
    {
        const auto& node = m_TerrainVisibleNodes[i];
        pInstanceData[i].NodeOffset = { (int)node->m_Origin.x, (int)node->m_Origin.y, 512, 1 };
    }
    RenderOption.piImmediateContext->Unmap(desc.pInstanceBuffer, 0);

    RenderOption.piImmediateContext->IASetVertexBuffers(1, 1, &desc.pInstanceBuffer, &nStride, &nOffset);
    RenderOption.piImmediateContext->DrawIndexedInstanced(desc.nIndies, m_TerrainVisibleNodes.size(), 0, 0, 0);
}

HRESULT L3DLandscape::Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable)
{
    LANDSCAPE_DESC desc{ pathTable.dir.c_str(), pathTable.mapName.c_str() };
    LANDSCAPE_SOURCE* pSource = nullptr;

    LoadLandscape(&desc, pSource);

    m_MaterialPack.resize(pSource->nMaterialCount);
    for (int i = 0; i < pSource->nMaterialCount; i++)
    {
        // Always use the LOD0
        assert(pSource->pMaterials[i].nLODCount > 0);
        const auto& material = pSource->pMaterials[i].pLOD[0];

        m_MaterialPack[i] = new L3DMaterial;
        m_MaterialPack[i]->Create(piDevice, material, RUNTIME_MACRO_TERRAIN);
    }

    m_Regions.resize(pSource->RegionTableSize.x);
    for (int i = 0; i < pSource->RegionTableSize.x; i++)
    {
        m_Regions[i].resize(pSource->RegionTableSize.y);
        for (int j = 0; j < pSource->RegionTableSize.y; j++)
        {
            const auto& region = pSource->pRegionTable[i * pSource->RegionTableSize.y + j];
            m_Regions[i][j] = new L3DLandscapeRegion;
            m_Regions[i][j]->m_Origin = { i * (int)pSource->RegionSize, j * (int)pSource->RegionSize };
            m_Regions[i][j]->Load(piDevice, region, *pSource, m_MaterialPack);
        }
    }

    m_uMinBlockLength = pSource->LeafNodeSize;
    m_TerrainCB.UnitScaleXZ = pSource->UnitScale;

    _BuildVertices(piDevice, m_uMinBlockLength + 1);
    _BuildIndices(piDevice, m_uMinBlockLength + 1);
    _BuildInstance(piDevice, m_uMinBlockLength + 1);

    SAFE_RELEASE(pSource);

    return S_OK;
}

void L3DLandscape::UpdateVisibility()
{
    m_VisibleRegions.clear();
    for (const auto& regionRow : m_Regions)
        for (const auto& region : regionRow)
        {
            region->UpdateVisiblity();
            m_VisibleRegions.push_back(region);
        }
}

void L3DLandscape::RenderTerrain(const SCENE_RENDER_OPTION& RenderOption, RENDER_PASS RenderPass)
{
    UINT nStride = 0;
    UINT nOffset = 0;

    nStride = sizeof(XMFLOAT2);

    RenderOption.piImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    RenderOption.piImmediateContext->IASetInputLayout(RenderOption.pShaderTable->Layout[L3D_INPUT_LAYOUT_CI_TERRAIN]);
    RenderOption.piImmediateContext->IASetIndexBuffer(m_IndiesBuffer, DXGI_FORMAT_R16_UINT, 0);
    RenderOption.piImmediateContext->IASetVertexBuffers(0, 1, &m_VertiesBuffer, &nStride, &nOffset);

    for (auto region : m_VisibleRegions)
    {
        RENDER_REGION_DESC desc{ m_TerrainCB, RenderPass, m_nIndies, m_InstanceBuffer };
        region->RenderTerrain(RenderOption, desc);
    }
}

void L3DLandscape::_BuildVertices(ID3D11Device* piDevice, UINT uNumVerticesPerEdge)
{
    D3D11_BUFFER_DESC           bufDesc;
    D3D11_SUBRESOURCE_DATA      bufData;

    UINT uNumVertex = uNumVerticesPerEdge * uNumVerticesPerEdge;
    XMFLOAT2* pVertexBuffer = new XMFLOAT2[uNumVertex];

    for (unsigned y = 0; y < uNumVerticesPerEdge; ++y)
    {
        for (unsigned x = 0; x < uNumVerticesPerEdge; ++x)
        {
            unsigned uIndex = y * uNumVerticesPerEdge + x;
            assert(uIndex < uNumVertex);

            pVertexBuffer[uIndex].x = (float)x;
            pVertexBuffer[uIndex].y = (float)y;
        }
    }

    bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufDesc.ByteWidth = uNumVertex * sizeof(XMFLOAT2);
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    bufDesc.StructureByteStride = 0;
    bufDesc.MiscFlags = 0;

    bufData.pSysMem = pVertexBuffer;
    bufData.SysMemPitch = 0;
    bufData.SysMemSlicePitch = 0;

    piDevice->CreateBuffer(&bufDesc, &bufData, &m_VertiesBuffer);

    SAFE_DELETE_ARRAY(pVertexBuffer);
}

#define ADD_INDEX(x, z, pitch) ((x) + (z) * (pitch))
void L3DLandscape::_BuildIndices(ID3D11Device* piDevice, UINT uNumVerticesPerEdge)
{
    D3D11_BUFFER_DESC bufDesc;
    D3D11_SUBRESOURCE_DATA bufData;

    m_nIndies = 0;
    UINT MinBlockLength = uNumVerticesPerEdge - 1;
    WORD* pIndexBuffer = new WORD[uNumVerticesPerEdge * uNumVerticesPerEdge * 6];

    for (unsigned y = 0; y < MinBlockLength; y++)
    {
        for (unsigned x = 0; x < MinBlockLength; x++)
        {
            pIndexBuffer[m_nIndies++] = (WORD)ADD_INDEX(x, y, uNumVerticesPerEdge);
            pIndexBuffer[m_nIndies++] = (WORD)ADD_INDEX(x + 1, y, uNumVerticesPerEdge);
            pIndexBuffer[m_nIndies++] = (WORD)ADD_INDEX(x, y + 1, uNumVerticesPerEdge);

            pIndexBuffer[m_nIndies++] = (WORD)ADD_INDEX(x + 1, y, uNumVerticesPerEdge);
            pIndexBuffer[m_nIndies++] = (WORD)ADD_INDEX(x + 1, y + 1, uNumVerticesPerEdge);
            pIndexBuffer[m_nIndies++] = (WORD)ADD_INDEX(x, y + 1, uNumVerticesPerEdge);
        }
    }

    bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufDesc.ByteWidth = m_nIndies * sizeof(WORD);
    bufDesc.CPUAccessFlags = 0;
    bufDesc.MiscFlags = 0;
    bufDesc.StructureByteStride = 0;
    bufDesc.Usage = D3D11_USAGE_IMMUTABLE;

    bufData.pSysMem = pIndexBuffer;
    bufData.SysMemPitch = 0;
    bufData.SysMemSlicePitch = 0;

    piDevice->CreateBuffer(&bufDesc, &bufData, &m_IndiesBuffer);
}

void L3DLandscape::_BuildInstance(ID3D11Device* piDevice, UINT uNumVerticesPerEdge)
{
    D3D11_BUFFER_DESC bufDesc;
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.ByteWidth = m_nIndies * sizeof(INSTANCE_DATA);
    bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufDesc.MiscFlags = 0;
    bufDesc.StructureByteStride = 0;
    bufDesc.Usage = D3D11_USAGE_DYNAMIC;

    piDevice->CreateBuffer(&bufDesc, nullptr, &m_InstanceBuffer);
}
