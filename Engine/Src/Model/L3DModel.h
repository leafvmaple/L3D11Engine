#pragma once

#include <windows.h>
#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>

#include "L3DInterface.h"

class L3DMesh;
class L3DMaterial;
class L3DRenderUnit;

class L3DModel : public ILModel
{
public:
    HRESULT Create(ID3D11Device* piDevice, const char* szFileName);

    L3DRenderUnit* m_pRenderUnit = nullptr;

private:
    L3DMesh* m_pMesh = nullptr;
    L3DMaterial* m_pMaterial = nullptr;
};