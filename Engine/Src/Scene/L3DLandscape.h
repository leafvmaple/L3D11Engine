#pragma once

#include "L3DInterface.h"

class L3DMaterial;

struct SCENE_PATH_TABLE;

class L3DLandscape
{
public:
    HRESULT Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);

private:
    std::vector<std::vector<L3DMaterial*>> materials;
};