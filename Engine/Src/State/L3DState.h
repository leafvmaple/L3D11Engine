#pragma once

#include <d3d11.h>

enum RASTERIZER_STATE_ID
{
	L3D_RASTERIZER_STATE_CULL_NONE,
	L3D_RASTERIZER_STATE_CULL_CLOCKWISE,

	L3D_RASTERIZER_STATE_COUNT,
};

enum SAMPLER_STATE_ID
{
	L3D_SAMPLER_STATE_DEFAULT,

	L3D_SAMPLER_STATE_POINT_CLAMP,

	L3D_SAMPLER_STATE_COUNT
};

enum DEPTHSTENCIL_STATE_ID
{
	//Z writing enable state
	L3D_ZWRITE_ENABLE,

	L3D_DEPTH_WRITE_COUNT
};

struct L3D_STATE_TABLE
{
	ID3D11RasterizerState*   Rasterizer[L3D_RASTERIZER_STATE_COUNT];
	ID3D11SamplerState*      Sampler[L3D_SAMPLER_STATE_COUNT];
	ID3D11DepthStencilState* DepthStencilState[L3D_DEPTH_WRITE_COUNT];
};

/*
* Create Rasterizer State
* Create Depth Stencil State
*/
L3D_STATE_TABLE* CreateStateTable(ID3D11Device* piDevice);