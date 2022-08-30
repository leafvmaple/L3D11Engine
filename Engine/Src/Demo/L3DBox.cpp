#include <d3dcompiler.h>
#include "FX11/inc/d3dx11effect.h"

#include "L3DBox.h"
#include "LAssert.h"

#pragma comment(lib, "D3DCompiler.lib")
#ifdef _DEBUG
#pragma comment(lib, "Effects11d.lib")
#endif

struct VertexInfo
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

HRESULT L3DBox::Create(ID3D11Device* piDevice)
{
	VertexInfo vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), Colors::White   },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), Colors::Black   },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), Colors::Red     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), Colors::Green   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), Colors::Blue    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), Colors::Yellow  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), Colors::Cyan    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), Colors::Magenta }
	};

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3D11_BUFFER_DESC vbd;
	D3D11_BUFFER_DESC ibd;
	D3D11_SUBRESOURCE_DATA vinitData;
	D3D11_SUBRESOURCE_DATA iinitData;

	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	DWORD dwShaderFlags = 0;
	ID3DBlob* pCompiledShader = 0;
	ID3DBlob* pCompilationMsgs = 0;
	ID3DX11Effect* pEffect = nullptr;
	ID3D11InputLayout* pInputLayout = nullptr;

	memset(&m_RenderParam, 0, sizeof(m_RenderParam));

	// Create vertex buffer
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexInfo) * _countof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	vinitData.pSysMem = vertices;

	piDevice->CreateBuffer(&vbd, &vinitData, &m_RenderParam.VertexBuffer.piBuffer);
	m_RenderParam.VertexBuffer.uStride = sizeof(VertexInfo);
	m_RenderParam.VertexBuffer.uOffset = 0;
	
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * _countof(indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	iinitData.pSysMem = indices;

	piDevice->CreateBuffer(&ibd, &iinitData, &m_RenderParam.IndexBuffer.piBuffer);
	m_RenderParam.IndexBuffer.eFormat = DXGI_FORMAT_R32_UINT;
	m_RenderParam.IndexBuffer.uOffset = 0;

	m_RenderParam.Draw.Indexed.uIndexCount = _countof(indices);
	m_RenderParam.Draw.Indexed.nBaseVertexLocation = 0;
	m_RenderParam.Draw.Indexed.uStartIndexLocation = 0;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
	dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	hr = D3DCompileFromFile(L"Res/Shader/color.fx", 0, 0, 0, "fx_5_0", dwShaderFlags, 0, &pCompiledShader, &pCompilationMsgs);
	HRESULT_ERROR_EXIT(hr);

	hr = D3DX11CreateEffectFromMemory(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), 0, piDevice, &pEffect);
	HRESULT_ERROR_EXIT(hr);

	m_pEffectTech = pEffect->GetTechniqueByName("ColorTech");
	BOOL_ERROR_EXIT(m_pEffectTech);

	m_pWorldViewProj = pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	BOOL_ERROR_EXIT(m_pWorldViewProj);

	m_RenderParam.eInputLayer = L3D_INPUT_LAYOUT_BOX;
	m_RenderParam.eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_RenderParam.pUnit = this;

	hResult = S_OK;
Exit0:
	SAFE_RELEASE(pCompiledShader);

	return hResult;
}

HRESULT L3DBox::Draw(ID3D11DeviceContext* pDeviceContext, XMMATRIX *pWorldViewProj)
{
	D3DX11_TECHNIQUE_DESC techDesc;

	m_pWorldViewProj->SetMatrix(reinterpret_cast<float*>(pWorldViewProj));
	m_pEffectTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffectTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
	}

	return S_OK;
}

HRESULT L3DBox::PushRenderUnit(std::vector<RENDER_STAGE_INPUT_ASSEMBLER>& vecRenderUnit)
{
	vecRenderUnit.push_back(m_RenderParam);

	return S_OK;
}
