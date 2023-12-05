#include "stdafx.h"
#include "L3DLandscape.h"

#include "L3DFormat.h"
#include "ILandscape.h"

HRESULT L3DLandscape::Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable)
{
    LANDSCAPE_DESC desc{ pathTable.dir.c_str(), pathTable.mapName.c_str() };
    LANDSCAPE_SOURCE* pSource = nullptr;

    LoadLandscape(&desc, pSource);

    return S_OK;
}
