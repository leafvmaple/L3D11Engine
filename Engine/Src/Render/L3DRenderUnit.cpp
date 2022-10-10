#include "stdafx.h"

#include "L3DRenderUnit.h"

#include "Model/L3DMaterial.h"
#include "Model/L3DShader.h"
#include "Model/L3DMaterialConfig.h"

#include "Camera/L3DCamera.h"

#include "FX11/inc/d3dx11effect.h"

HRESULT _DrawElement(const SCENE_RENDER_OPTION& Param, const RENDER_STAGE_INPUT_ASSEMBLER& Stage)
{
    ID3D11InputLayout* piInputLayout = nullptr;

    piInputLayout = Param.pShaderTable->Layout[Stage.eInputLayer];
    Param.piImmediateContext->IASetInputLayout(piInputLayout);

	Param.piImmediateContext->IASetPrimitiveTopology(Stage.eTopology);

    Param.piImmediateContext->IASetVertexBuffers(0, 1, &Stage.VertexBuffer.piBuffer, &Stage.VertexBuffer.uStride, &Stage.VertexBuffer.uOffset);
    Param.piImmediateContext->IASetIndexBuffer(Stage.IndexBuffer.piBuffer, Stage.IndexBuffer.eFormat, Stage.IndexBuffer.uOffset);

    Param.piImmediateContext->DrawIndexed(Stage.Draw.Indexed.uIndexCount, Stage.Draw.Indexed.uStartIndexLocation, Stage.Draw.Indexed.nBaseVertexLocation);

    return S_OK;
}

HRESULT L3DRenderUnit::ApplyProcess(const SCENE_RENDER_OPTION& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    MESH_SHARED_CB MeshCB;

    XMMATRIX viewProj = Param.pCamera->GetViewProjcetion();
    XMStoreFloat4x4(&MeshCB.MatrixWorld, m_MatrixWorld * viewProj);

    hr = m_pMaterial->Apply(Param.piImmediateContext);
    HRESULT_ERROR_EXIT(hr);

	m_ModelVariables.pModelParams->SetRawValue(&MeshCB, 0, sizeof(MESH_SHARED_CB));

    hr = _DrawElement(Param, m_IAStage);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

void L3DRenderUnit::SetWorldMatrix(const XMFLOAT4X4& World)
{
    m_MatrixWorld = XMLoadFloat4x4(&World);
}
