#pragma once

#include "L3DInterface.h"
#include "L3DFormat.h"
#include "Model/L3DMaterialConfig.h"

class L3DMaterial;
class L3DTexture;

struct SCENE_PATH_TABLE;
struct SCENE_RENDER_OPTION;

struct LANDSCAPE_REGION;
struct LANDSCAPE_SOURCE;

struct MTLSYS_TERRAIN_CB;

struct RENDER_REGION_DESC;

interface ID3DX11EffectVariable;

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

    void GetFloor(XMFLOAT2& vPos, float& fHeight);

private:
    std::vector<L3DMaterial*> m_Material;
    std::vector<L3DLandscapeTerrainNode*> m_TerrainVisibleNodes;

    UINT m_nSize = 0;
    UINT m_nHeightMapSize = 0;

    L3DMaterial* m_pMaterial = nullptr;
    L3DTexture* m_pHeightMap = nullptr;
    L3DLandscapeTerrainNode* m_pTerrainNode = nullptr;

    ID3DX11EffectVariable* m_pModelParams = nullptr;

    std::vector<float> m_HeightData;
};

class L3DLandscape
{
public:
    HRESULT Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);
    void UpdateVisibility();
    void RenderTerrain(const SCENE_RENDER_OPTION& RenderOption, RENDER_PASS RenderPass);

    void GetFloor(float fX, float fY, float& fHeight);

private:
    std::vector<L3DMaterial*> m_MaterialPack;
    std::vector<L3DLandscapeRegion*> m_Regions;
    std::vector<L3DLandscapeRegion*> m_VisibleRegions;

    MTLSYS_TERRAIN_CB m_TerrainCB = {};
    ID3D11Buffer* m_VertiesBuffer = nullptr;
    ID3D11Buffer* m_IndiesBuffer = nullptr;
    ID3D11Buffer* m_InstanceBuffer = nullptr;

    UINT m_nMinBlockLength = 0;
    UINT m_nRegionTableWidth = 0;
    UINT m_nRegionSize = 0;

private:
    void _BuildVertices(ID3D11Device* piDevice, UINT uNumVerticesPerEdge);
    void _BuildIndices(ID3D11Device* piDevice, UINT uNumVerticesPerEdge);
    void _BuildInstance(ID3D11Device* piDevice, UINT uNumVerticesPerEdge);

    UINT m_nIndies = 0;
};