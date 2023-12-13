#pragma once

#include "LInterface.h"
#include "LAssert.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include "IMaterial.h"

using namespace DirectX;

struct LANDSCAPE_DESC
{
    const wchar_t* szDir = nullptr;
    const wchar_t* szMapName = nullptr;
};

struct LANDSCAPE_REGION
{
    int nMaterial = 0;
    int* pMaterialIDs = nullptr;

    int nHeightData = 0;
    float* pHeightData = nullptr;

    ~LANDSCAPE_REGION() {
        SAFE_DELETE_ARRAY(pMaterialIDs);
        SAFE_DELETE_ARRAY(pHeightData);
    }
};

struct LANDSCAPE_MATERIAL
{
    int nLODCount = 0;
    MATERIAL_SOURCE* pLOD = nullptr; // every LOD has a material
};

struct LANDSCAPE_SOURCE : LUnknown
{
    UINT        RegionSize;
    UINT        LeafNodeSize;
    XMFLOAT2   WorldOrigin;
    XMFLOAT2   UnitScale;

    XMUINT2     RegionTableSize;

    int nMaterialCount = 0;
    LANDSCAPE_MATERIAL* pMaterials = nullptr;

    LANDSCAPE_REGION* pRegionTable = nullptr;

    virtual ~LANDSCAPE_SOURCE() {
        SAFE_DELETE_ARRAY(pRegionTable);
        SAFE_DELETE_ARRAY(pMaterials);
    }
};

L3DENGINE_API void LoadLandscape(LANDSCAPE_DESC* pDesc, LANDSCAPE_SOURCE*& pSource);