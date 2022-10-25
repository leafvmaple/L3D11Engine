#pragma once

#include <d3d11.h>

enum DEPTHSTENCIL_STATE_ID
{
	//Z writing enable state
	L3D_ZWRITE_ENABLE,

	L3D_DEPTH_WRITE_COUNT
};

struct L3D_STATE_TABLE
{
	ID3D11DepthStencilState* DepthStencilState[L3D_DEPTH_WRITE_COUNT];
};

L3D_STATE_TABLE* CreateStateTable(ID3D11Device* piDevice);