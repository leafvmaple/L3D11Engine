#pragma once

#include <vector>

#include "L3DInterface.h"

class L3DRenderUnit;

enum L3D_INPUT_LAYOUT
{
	L3D_INPUT_LAYOUT_BOX,
	L3D_INPUT_LAYOUT_CI_SKINMESH,

	L3D_INPUT_LAYOUT_COUNT,
};

enum L3D_PIXEL_SHADER
{
	L3D_PIXEL_SHADER_NULL,
	L3D_PIXEL_SHADER_UV1_NO_LIGHT,

	L3D_PIXEL_SHADER_COUNT,
};

enum L3D_VERTEX_SHADER
{
	L3D_VERTEX_SHADER_BOX,
	L3D_VERTEX_SHADER_CI_SKINMESH,

	L3D_VERTEX_SHADER_COUNT,
};

struct RENDER_STATE_DRAW
{
	struct DRAW_INDEXED
	{
		UINT uIndexCount;
		UINT uStartIndexLocation;
		INT  nBaseVertexLocation;
	} Indexed;
};

struct RENDER_STAGE_INPUT_ASSEMBLER
{

	L3D_INPUT_LAYOUT eInputLayer;
	D3D11_PRIMITIVE_TOPOLOGY eTopology;

	struct VERTEX_BUFFER
	{
		ID3D11Buffer* piBuffer;
		UINT uStride;
		UINT uOffset;
	} VertexBuffer;

	struct INDEX_BUFFER
	{
		ID3D11Buffer* piBuffer;
		DXGI_FORMAT eFormat;
		UINT uOffset;
	} IndexBuffer;

	RENDER_STATE_DRAW Draw;

	L3DRenderUnit *pUnit;
};

class L3DRenderUnit
{
public:
	virtual HRESULT Draw(ID3D11DeviceContext* pDeviceContext, XMMATRIX* pWorldViewProj) = 0;
	virtual HRESULT PushRenderUnit(std::vector<RENDER_STAGE_INPUT_ASSEMBLER>& vecRenderUnit) = 0;

protected:
	RENDER_STAGE_INPUT_ASSEMBLER m_RenderParam;
};