#include "ILandscape.h"
#include "LFileReader.h"

#include <filesystem>
#include <format>

#include "rapidjson/include/rapidjson/document.h"

void _LoadLandscapeRegion(LANDSCAPE_REGION_SOURCE& region, const wchar_t* filename)
{
    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(filename, JsonDocument);

    {
        auto MaterialArray = JsonDocument["MaterialID"].GetArray();
        region.nMaterialCount = MaterialArray.Size();
        region.pMaterial = new LANDSCAPE_MATERIAL_SOURCE[region.nMaterialCount];

        for (int i = 0; i < region.nMaterialCount; i++)
        {
            region.pMaterial[i].nMaterialID = MaterialArray[i].GetUint64();
        }
    }
}

void _LoadLandscapeMaterial(const wchar_t* szFileName)
{
    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(szFileName, JsonDocument);


}

void _LoadLandscapeInfo(LANDSCAPE_SOURCE*& pSource, const wchar_t* szFileName)
{
    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(szFileName, JsonDocument);

    RapidJsonGet(pSource->RegionTableSize.x, JsonDocument, "RegionTableSize.x");
    RapidJsonGet(pSource->RegionTableSize.y, JsonDocument, "RegionTableSize.y");

    RapidJsonGet(pSource->RegionSize, JsonDocument, "RegionSize", 512);

    RapidJsonGet(pSource->WorldOrigin.x, JsonDocument, "WorldOrigin.x", 512);
    RapidJsonGet(pSource->WorldOrigin.y, JsonDocument, "WorldOrigin.y", 512);

    RapidJsonGet(pSource->UnitScale.x, JsonDocument, "UnitScale.x", 512);
    RapidJsonGet(pSource->UnitScale.y, JsonDocument, "UnitScale.y", 512);
}

void LoadLandscape(LANDSCAPE_DESC* pDesc, LANDSCAPE_SOURCE*& pSource)
{
    wchar_t szFileName[MAX_PATH];
    wsprintf(szFileName, L"%s/landscape/%s_landscapeinfo.json", pDesc->szDir, pDesc->szMapName);

    pSource = new LANDSCAPE_SOURCE;
    pSource->AddRef();

    _LoadLandscapeInfo(pSource, szFileName);

    pSource->pRegionTable = new LANDSCAPE_REGION_SOURCE[pSource->RegionTableSize.x * pSource->RegionTableSize.y];

    for (int i = 0; i < pSource->RegionTableSize.x; i++)
    {
        for (int j = 0; j < pSource->RegionTableSize.y; j++)
        {
            wsprintf(szFileName, L"%s/landscape/regioninfo/%s_%03d_%03d.json", pDesc->szDir, pDesc->szMapName, i, j);
            _LoadLandscapeRegion(pSource->pRegionTable[i * pSource->RegionTableSize.y + j], szFileName);
        }
    }
}
