#include "stdafx.h"

#include <wtypes.h>
#include <atlconv.h>

#include "L3DShader.h"
#include "IO/LFileReader.h"

#define FXO_PATH "Res/Shader/Base/FXO/g_%s_%s_ASSEMBLY.fxo"

static const char* VERTEX_SHADER_ID_NAME[] =
{
	"VERTEX_SHADER_BOX",
	"VERTEX_SHADER_CI_SKINMESH",
};

static const char* PIXEL_SHADER_ID_NAME[] =
{
	"PIXEL_SHADER_NULL",
	"PIXEL_SHADER_UV1_NO_LIGHT",
};

HRESULT LoadShader(const char* szFileName, const char* szType, BYTE** ppByteCode, size_t* puSize)
{
	int nRetCode = false;
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	GUID idFile = GUID_NULL;
	char szFilePath[MAX_PATH] = "\0";

	nRetCode = sprintf_s(szFilePath, FXO_PATH, szType, szFileName);
	BOOL_ERROR_EXIT(nRetCode);

	hr = LFileReader::Read(szFilePath, ppByteCode, puSize);
	HRESULT_ERROR_EXIT(hr);

	*puSize -= sizeof(GUID);

	hResult = S_OK;
Exit0:
	return hResult;
}

L3D_SHADER_TABLE* CreateShaderTable(ID3D11Device* piDevice)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	BYTE* pByte = nullptr;
	size_t uSize = 0;

	L3D_SHADER_TABLE* pShaderTable = nullptr;

	struct _SHADER_INFO
	{
		BYTE* pByte;
		size_t nSize;
	} VertexShaderInfos[L3D_VERTEX_SHADER_COUNT] = { 0 };

	pShaderTable = new L3D_SHADER_TABLE;
	BOOL_ERROR_EXIT(pShaderTable);

	// Vertex Shader
	for (int i = 0; i < L3D_VERTEX_SHADER_COUNT; i++)
	{
		_SHADER_INFO& ShaderInfo = VertexShaderInfos[i];

		hr = LoadShader(VERTEX_SHADER_ID_NAME[i], "vs", &ShaderInfo.pByte, &ShaderInfo.nSize);
		HRESULT_ERROR_EXIT(hr);

		hr = piDevice->CreateVertexShader(ShaderInfo.pByte, ShaderInfo.nSize, nullptr, &pShaderTable->Vertex[i]);
		HRESULT_ERROR_EXIT(hr);
	}

	// Pixel Shader
	pShaderTable->Pixel[L3D_PIXEL_SHADER_NULL] = nullptr;
	for (int i = L3D_PIXEL_SHADER_NULL + 1; i < L3D_PIXEL_SHADER_COUNT; i++)
	{
		hr = LoadShader(PIXEL_SHADER_ID_NAME[i], "ps", &pByte, &uSize);
		HRESULT_ERROR_EXIT(hr);

		hr = piDevice->CreatePixelShader(pByte, uSize, nullptr, &pShaderTable->Pixel[i]);
		HRESULT_ERROR_EXIT(hr);
	}

	// Input Layer
	for (int i = 0; i < L3D_INPUT_LAYOUT_COUNT; i++)
	{
		const _LAYOUT_INFO& LayoutInfo = INPUT_LAYOUT_LIST[i];

		hr = piDevice->CreateInputLayout(
			LayoutInfo.DescArr,
			LayoutInfo.uDescCount,
			VertexShaderInfos[LayoutInfo.eVertexShader].pByte,
			VertexShaderInfos[LayoutInfo.eVertexShader].nSize,
			&pShaderTable->Layout[i]
		);
		HRESULT_ERROR_EXIT(hr);
	}

	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
	{
		SAFE_DELETE(pShaderTable);
	}
	return pShaderTable;
}
