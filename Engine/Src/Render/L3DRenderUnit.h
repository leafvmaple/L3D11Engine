#pragma once

#include <d3d11.h>
#include <windows.h>

#include "L3DFormat.h"

struct L3D_SHADER_TABLE;

class L3DMaterial;
class L3DCamera;

struct SCENE_RENDER_OPTION
{
    ID3D11Device* piDevice;
    ID3D11DeviceContext* piImmediateContext;

    L3D_SHADER_TABLE* pShaderTable;

    L3DCamera* pCamera;
};

class L3DRenderUnit
{
public:
    HRESULT Create(const RENDER_STAGE_INPUT_ASSEMBLER& Stage, L3DMaterial* pMaterial);
    HRESULT ApplyProcess(const SCENE_RENDER_OPTION& Param);

    void SetWorldMatrix(const XMMATRIX& world);

    RENDER_STAGE_INPUT_ASSEMBLER m_IAStage;

    L3DMaterial* m_pMaterial = nullptr;

private:
    XMMATRIX m_MatrixWorld = XMMatrixIdentity();
};