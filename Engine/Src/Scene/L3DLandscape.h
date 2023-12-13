#pragma once

#include "L3DInterface.h"
#include "L3DFormat.h"
#include "Model/L3DMaterialConfig.h"

class L3DMaterial;

struct SCENE_PATH_TABLE;
struct SCENE_RENDER_OPTION;

struct LANDSCAPE_REGION;
struct LANDSCAPE_SOURCE;

struct MTLSYS_TERRAIN_CB;

struct RENDER_REGION_DESC;

class L3DLandscapeTerrainNode
{
public:
    XMINT2 m_Origin;
public:
    HRESULT Load(ID3D11Device* piDevice, const LANDSCAPE_SOURCE& source);
    void GetVisibleNodes(std::vector<L3DLandscapeTerrainNode*>& VisibleNodes);

private:
    void _CreateChildNodes(const LANDSCAPE_SOURCE& source);

    std::vector<L3DLandscapeTerrainNode*> m_Children;
};

class L3DLandscapeRegion
{
public:
    XMINT2 m_Origin;
public:
    HRESULT Load(ID3D11Device* piDevice, const LANDSCAPE_REGION& region, const LANDSCAPE_SOURCE& source, const std::vector<L3DMaterial*>& materialPack);
    void UpdateVisiblity();
    void RenderTerrain(const SCENE_RENDER_OPTION& RenderOption, const RENDER_REGION_DESC& desc);

private:
    std::vector<L3DMaterial*> m_Material;
    std::vector<L3DLandscapeTerrainNode*> m_TerrainVisibleNodes;

    ID3D11Texture2D* m_pHeightMapTexture = nullptr;
    ID3D11ShaderResourceView* m_pHeightMapSRV = nullptr;

    L3DMaterial* m_pMaterial = nullptr;
    L3DLandscapeTerrainNode* m_pTerrainNode = nullptr;
};

class L3DLandscape
{
public:
    HRESULT Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);
    void UpdateVisibility();
    void RenderTerrain(const SCENE_RENDER_OPTION& RenderOption, RENDER_PASS RenderPass);

private:
    std::vector<L3DMaterial*> m_MaterialPack;
    std::vector<std::vector<L3DLandscapeRegion*>> m_Regions;
    std::vector<L3DLandscapeRegion*> m_VisibleRegions;

    MTLSYS_TERRAIN_CB m_TerrainCB = {};
    ID3D11Buffer* m_VertiesBuffer = nullptr;
    ID3D11Buffer* m_IndiesBuffer = nullptr;
    ID3D11Buffer* m_InstanceBuffer = nullptr;

    UINT m_uMinBlockLength = 0;

private:
    void _BuildVertices(ID3D11Device* piDevice, UINT uNumVerticesPerEdge);
    void _BuildIndices(ID3D11Device* piDevice, UINT uNumVerticesPerEdge);
    void _BuildInstance(ID3D11Device* piDevice, UINT uNumVerticesPerEdge);

    UINT m_nIndies = 0;
};