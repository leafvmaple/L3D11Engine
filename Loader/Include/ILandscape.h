#pragma once

#include "LInterface.h"
#include "LAssert.h"

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct LANDSCAPE_DESC
{
    const wchar_t* szDir = nullptr;
    const wchar_t* szMapName = nullptr;
};

struct LANDSCAPE_MATERIAL_SOURCE
{
    int nMaterialID = 0;
};

struct LANDSCAPE_REGION_SOURCE
{
    int nMaterialCount = 0;

    LANDSCAPE_MATERIAL_SOURCE* pMaterial = nullptr;

    ~LANDSCAPE_REGION_SOURCE() {
        SAFE_DELETE_ARRAY(pMaterial);
    }
};

struct LANDSCAPE_SOURCE : LUnknown
{
    UINT        RegionSize;
    XMFLOAT2    WorldOrigin;
    XMFLOAT2    UnitScale;

    XMUINT2     RegionTableSize;

    LANDSCAPE_REGION_SOURCE* pRegionTable = nullptr;

    virtual ~LANDSCAPE_SOURCE() {
        SAFE_DELETE_ARRAY(pRegionTable);
    }
};

L3DENGINE_API void LoadLandscape(LANDSCAPE_DESC* pDesc, LANDSCAPE_SOURCE*& pSource);