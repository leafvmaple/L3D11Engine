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

    virtual HRESULT ResetTransform() override;
    virtual HRESULT SetTranslation(const XMFLOAT3& Translation) override;
    virtual HRESULT SetRotation(const XMFLOAT4& Rotation) override;
    virtual HRESULT SetScale(const XMFLOAT3& Scale) override;

    L3DRenderUnit* m_pRenderUnit = nullptr;

private:
    void UpdateTransFrom();

    L3DMesh* m_pMesh = nullptr;
    L3DMaterial* m_pMaterial = nullptr;

    XMFLOAT3 m_Translation;
    XMFLOAT4 m_Rotation;
    XMFLOAT3 m_Scale;

    XMFLOAT4X4 m_World;
};