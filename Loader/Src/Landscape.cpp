#include <filesystem>
#include <format>

#include "rapidjson/include/rapidjson/document.h"

#include "LFileReader.h"
#include "ILandscape.h"
#include "Material.h"

void _LoadLandscapeRegion(LANDSCAPE_REGION& region, const wchar_t* filename)
{
    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(filename, JsonDocument);

    {
        auto MaterialArray = JsonDocument["MaterialID"].GetArray();
        region.nMaterial = MaterialArray.Size();
        region.pMaterialIDs = new int[region.nMaterial];

        for (int i = 0; i < region.nMaterial; i++)
            region.pMaterialIDs[i] = MaterialArray[i].GetUint64();
    }
}

void _LoadLandscapeHeight(LANDSCAPE_REGION& region, const wchar_t* filename)
{
    BYTE* pHeightData = nullptr;
    size_t nSize = 0;

    LFileReader::Read(filename, &pHeightData, &nSize);
    region.nHeightData = sqrt(nSize / sizeof(float));

    region.pHeightData = new float[region.nHeightData * region.nHeightData];
    memcpy(region.pHeightData, pHeightData, region.nHeightData * region.nHeightData * sizeof(float));
}

void _LoadLandscapeMaterial(LANDSCAPE_SOURCE*& pSource, const wchar_t* szFileName)
{
    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(szFileName, JsonDocument);

    const auto& Materials = JsonDocument["Materials"];
    if (Materials.IsArray())
    {
        pSource->nMaterialCount = Materials.Size();
        pSource->pMaterials = new LANDSCAPE_MATERIAL[pSource->nMaterialCount];

        for (int i = 0; i < pSource->nMaterialCount; i++)
        {
            const auto& LODs = Materials[i]["LOD"];
            auto& material = pSource->pMaterials[i];

            material.nLODCount = LODs.Size();
            pSource->pMaterials[i].pLOD = new MATERIAL_SOURCE[material.nLODCount];

            for (int j = 0; j < material.nLODCount; j++)
            {
                _LoadMaterial(LODs[j], material.pLOD[j]);
            }
        }
    }
}

void _LoadLandscapeInfo(LANDSCAPE_SOURCE*& pSource, const wchar_t* szFileName)
{
    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(szFileName, JsonDocument);

    RapidJsonGet(pSource->RegionTableSize.x, JsonDocument, "RegionTableSize.x");
    RapidJsonGet(pSource->RegionTableSize.y, JsonDocument, "RegionTableSize.y");

    RapidJsonGet(pSource->RegionSize, JsonDocument, "RegionSize", 512);
    RapidJsonGet(pSource->LeafNodeSize, JsonDocument, "LeafNodeSize", 64);

    RapidJsonGet(pSource->WorldOrigin.x, JsonDocument, "WorldOrigin.x", 512);
    RapidJsonGet(pSource->WorldOrigin.y, JsonDocument, "WorldOrigin.y", 512);

    RapidJsonGet(pSource->UnitScale.x, JsonDocument, "UnitScale.x", 512);
    RapidJsonGet(pSource->UnitScale.y, JsonDocument, "UnitScale.y", 512);
}

void LoadLandscape(LANDSCAPE_DESC* pDesc, LANDSCAPE_SOURCE*& pSource)
{
    wchar_t szFileName[MAX_PATH];

    pSource = new LANDSCAPE_SOURCE;
    pSource->AddRef();

    wsprintf(szFileName, L"%s/landscape/%s_landscapeinfo.json", pDesc->szDir, pDesc->szMapName);
    _LoadLandscapeInfo(pSource, szFileName);

    wsprintf(szFileName, L"%s/landscape/%s_materials.json", pDesc->szDir, pDesc->szMapName);
    _LoadLandscapeMaterial(pSource, szFileName);

    pSource->pRegionTable = new LANDSCAPE_REGION[pSource->RegionTableSize.x * pSource->RegionTableSize.y];

    for (int i = 0; i < pSource->RegionTableSize.x; i++)
    {
        for (int j = 0; j < pSource->RegionTableSize.y; j++)
        {
            auto& region = pSource->pRegionTable[i * pSource->RegionTableSize.y + j];

            wsprintf(szFileName, L"%s/landscape/regioninfo/%s_%03d_%03d.json", pDesc->szDir, pDesc->szMapName, i, j);
            _LoadLandscapeRegion(region, szFileName);

            wsprintf(szFileName, L"%s/landscape/heightmap/%s_%03d_%03d.r32", pDesc->szDir, pDesc->szMapName, i, j);
            _LoadLandscapeHeight(region, szFileName);
        }
    }
}
