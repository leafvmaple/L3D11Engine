#pragma once

#include "L3DInterface.h"

struct SCENE_PATH_TABLE;

class L3DLandscape
{
public:
    HRESULT Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);
};