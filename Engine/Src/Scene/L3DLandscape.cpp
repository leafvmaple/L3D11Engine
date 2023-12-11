#include "stdafx.h"
#include "L3DLandscape.h"

#include "L3DFormat.h"
#include "ILandscape.h"

#include "Model/L3DMaterial.h"

HRESULT L3DLandscape::Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable)
{
    LANDSCAPE_DESC desc{ pathTable.dir.c_str(), pathTable.mapName.c_str() };
    LANDSCAPE_SOURCE* pSource = nullptr;

    LoadLandscape(&desc, pSource);

    materials.resize(pSource->nMaterialCount);
    for (int i = 0; i < pSource->nMaterialCount; i++)
    {
        const auto& material = pSource->pMaterials[i];

        materials[i].resize(material.nLODCount);
        for (int j = 0; j < material.nLODCount; j++)
        {
            const auto& LOD = material.pLOD[j];

            L3DMaterial* pMaterial = materials[i].emplace_back(new L3DMaterial);
            pMaterial->Create(piDevice, LOD, RUNTIME_MACRO_TERRAIN);
        }
    }

    return S_OK;
}
