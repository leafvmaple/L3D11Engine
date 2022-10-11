#pragma once

#include <d3d11.h>
#include <windows.h>

#include "L3DFormat.h"

struct L3D_SHADER_TABLE;

class L3DMaterial;
class L3DCamera;

struct ID3DX11EffectVariable;
struct ID3DX11EffectConstantBuffer;

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
	struct MODEL_FIX_VARIBLES
	{
		ID3DX11EffectVariable* pModelParams;
	};

    HRESULT ApplyProcess(const SCENE_RENDER_OPTION& Param, RENDER_PASS ePass);

    void SetWorldMatrix(const XMFLOAT4X4& World);

    RENDER_STAGE_INPUT_ASSEMBLER m_IAStage;

	ID3DX11EffectConstantBuffer* m_piModelSharedCB = nullptr;
	MODEL_FIX_VARIBLES m_ModelVariables;

    L3DMaterial* m_pMaterial = nullptr;

private:
    XMMATRIX m_MatrixWorld = XMMatrixIdentity();
};