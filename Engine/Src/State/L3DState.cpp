#include "stdafx.h"

#include "L3DState.h"

static const struct _DEPTH_STENCIL_STATE_INFO
{
	DEPTHSTENCIL_STATE_ID eID;
	D3D11_DEPTH_STENCIL_DESC Desc;
} DEPTH_STENCIL_STATE_LIST[] =
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
