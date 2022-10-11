#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct MESH_SHARED_CB
{
    DirectX::XMFLOAT4X4 MatrixWorld;
};

enum RUNTIME_MACRO
{
	RUNTIME_MACRO_MESH = 0,
	RUNTIME_MACRO_SKIN_MESH = 1,
};
