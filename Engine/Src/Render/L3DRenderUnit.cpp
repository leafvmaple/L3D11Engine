#include "stdafx.h"

#include "L3DRenderUnit.h"

#include "Model/L3DMaterial.h"
#include "Model/L3DShader.h"
#include "State/L3DState.h"

#include "Camera/L3DCamera.h"

#include "FX11/inc/d3dx11effect.h"

HRESULT _DrawElement(const SCENE_RENDER_OPTION& Param, const RENDER_STAGE_INPUT_ASSEMBLER& Stage)
{
    ID3D11InputLayout* piInputLayout = nullptr;

    piInputLayout = Param.pShaderTable->Layout[Stage.eInputLayout];
    Param.piImmediateContext->IASetInputLayout(piInputLayout);
    Param.piImmediateContext->IASetPrimitiveTopology(Stage.eTopology);

    Param.piImmediateContext->IASetVertexBuffers(0, 1, &Stage.VertexBuffer.piBuffer, &Stage.VertexBuffer.uStride, &Stage.VertexBuffer.uOffset);
    Param.piImmediateContext->IASetIndexBuffer(Stage.IndexBuffer.piBuffer, Stage.IndexBuffer.eFormat, Stage.IndexBuffer.uOffset);

    Param.piImmediateContext->DrawIndexed(Stage.Draw.Indexed.uIndexCount, Stage.Draw.Indexed.uStartIndexLocation, Stage.Draw.Indexed.nBaseVertexLocation);

    return S_OK;
}

HRESULT L3DRenderUnit::ApplyProcess(const SCENE_RENDER_OPTION& Param, RENDER_PASS ePass)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    MESH_SHARED_CB MeshCB;
    XMMATRIX* pBoneMatrices = nullptr;

    Param.piImmediateContext->OMSetDepthStencilState(Param.pStateTable->DepthStencilState[L3D_ZWRITE_ENABLE], 0xff);

    hr = m_pMaterial->Apply(Param.piImmediateContext, ePass);
    HRESULT_ERROR_EXIT(hr);

    hr = _DrawElement(Param, m_IAStage);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}
