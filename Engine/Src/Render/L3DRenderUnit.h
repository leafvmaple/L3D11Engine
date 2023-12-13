#pragma once

#include <d3d11.h>
#include <windows.h>
#include <memory>

#include "L3DFormat.h"

struct L3D_SHADER_TABLE;
struct L3D_STATE_TABLE;

class L3DMaterial;
class L3DCamera;

struct ID3DX11EffectVariable;
struct ID3DX11EffectConstantBuffer;

struct SCENE_RENDER_OPTION
{
    ID3D11Device* piDevice  = nullptr;
    ID3D11DeviceContext* piImmediateContext = nullptr;

    L3D_SHADER_TABLE* pShaderTable = nullptr;
    L3D_STATE_TABLE*  pStateTable  = nullptr;
};

class L3DRenderUnit
{
public:
    struct MODEL_FIX_VARIBLES
    {
        ID3DX11EffectVariable* pCustomMatrixBones = nullptr;
        ID3DX11EffectVariable* pModelParams = nullptr;
    };

    HRESULT ApplyProcess(const SCENE_RENDER_OPTION& Param, RENDER_PASS ePass);

    RENDER_STAGE_INPUT_ASSEMBLER m_IAStage;
    MODEL_FIX_VARIBLES m_ModelVariables;

    L3DMaterial* m_pMaterial = nullptr;

private:
    XMMATRIX m_MatrixWorld = XMMatrixIdentity();
};