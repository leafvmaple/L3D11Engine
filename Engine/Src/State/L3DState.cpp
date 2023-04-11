#include "stdafx.h"

#include "L3DState.h"


struct _RASTERIZER_STATE_INFO
{
	RASTERIZER_STATE_ID eID;
	D3D11_RASTERIZER_DESC Desc;
};

struct _DEPTH_STENCIL_STATE_INFO
{
	DEPTHSTENCIL_STATE_ID eID;
	D3D11_DEPTH_STENCIL_DESC Desc;
};

static const _RASTERIZER_STATE_INFO RASTERIZER_STATE_LIST[] =
{
	{
		L3D_RASTERIZER_STATE_CULL_CLOCKWISE,
		{
			D3D11_FILL_SOLID,   //D3D11_FILL_MODE FillMode;
			D3D11_CULL_BACK,    //D3D11_CULL_MODE CullMode;
			TRUE,               //BOOL FrontCounterClockwise;
			0,      //INT DepthBias;
			0.0f,   //FLOAT DepthBiasClamp;
			0.0f,   //FLOAT SlopeScaledDepthBias;
			TRUE,   //BOOL DepthClipEnable;
			FALSE,  //BOOL ScissorEnable;
			FALSE,  //BOOL MultisampleEnable;
			FALSE,  //BOOL AntialiasedLineEnable;
		}
	},
};

static const _DEPTH_STENCIL_STATE_INFO DEPTH_STENCIL_STATE_LIST[] =
{
	{
		L3D_ZWRITE_ENABLE,
		{
			TRUE,
			D3D11_DEPTH_WRITE_MASK_ALL,
			D3D11_COMPARISON_LESS_EQUAL,
			FALSE
		}
	},
};

L3D_STATE_TABLE* CreateStateTable(ID3D11Device* piDevice)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	L3D_STATE_TABLE* pTable = nullptr;

	pTable = new L3D_STATE_TABLE;

	for (int i = 0; i < L3D_RASTERIZER_STATE_COUNT; i++)
	{
		hr = piDevice->CreateRasterizerState(&RASTERIZER_STATE_LIST[i].Desc, &pTable->Rasterizer[i]);
		HRESULT_ERROR_EXIT(hr);
	}

	for (int i = 0; i < L3D_DEPTH_WRITE_COUNT; i++)
	{
		hr = piDevice->CreateDepthStencilState(&DEPTH_STENCIL_STATE_LIST[i].Desc, &pTable->DepthStencilState[i]);
		HRESULT_ERROR_EXIT(hr);
	}

	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
		SAFE_DELETE(pTable);
	return pTable;
}
