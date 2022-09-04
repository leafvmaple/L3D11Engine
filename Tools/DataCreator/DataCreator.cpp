// ShaderCompiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <cstdio>

#include "LAssert.h"
#include "L3DFormat.h"

using namespace DirectX;

struct VertexInfo
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

int CreateMesh(XMFLOAT3* pVertext, XMCOLOR* pColor, int nVerticesCount, DWORD* pIndices, int nIndicesCount)
{
	size_t nFaceNum = nIndicesCount / 3;
	size_t nSizeHeader   = sizeof(_MESH_FILE_HEAD);
	size_t nSizePosition = sizeof(XMFLOAT3) * nVerticesCount;
	size_t nSizeColor    = sizeof(XMCOLOR) * nVerticesCount;
	size_t nSizeIndices  = sizeof(DWORD) * nIndicesCount;
	size_t nSizeSubset   = sizeof(WORD) * nFaceNum;
	size_t nOffset = 0;

	FILE* pFile = nullptr;
	BYTE* pbyData = nullptr;
	WORD* pSubset = nullptr;
	_MESH_FILE_HEAD *pMesh = nullptr;

	pFile = fopen("../../Client/Res/Mesh/Box.mesh", "wb + ");

	pbyData = new BYTE[nSizeHeader + nSizePosition + nSizeColor + nSizeIndices + nSizeSubset];

	pMesh = (_MESH_FILE_HEAD*)pbyData;
	memset(pbyData, 0, sizeof(_MESH_FILE_HEAD));

	pMesh->VersionHead.dwFileMask = _MESH_FILE_VERSION_HEAD::s_dwFileMask;

	pMesh->MeshHead.dwNumVertices = nVerticesCount;
	pMesh->MeshHead.dwNumFaces = nFaceNum;

	nOffset += nSizeHeader;

	pMesh->MeshHead.Blocks.PositionBlock = nOffset;
	memcpy(pbyData + nOffset, pVertext, nSizePosition);
	nOffset += nSizePosition;

	pMesh->MeshHead.Blocks.DiffuseBlock = nOffset;
	memcpy(pbyData + nOffset, pColor, nSizeColor);
	nOffset += nSizeColor;

	pMesh->MeshHead.Blocks.FacesIndexBlock = nOffset;
	memcpy(pbyData + nOffset, pIndices, nSizeIndices);
	nOffset += nSizeIndices;

	pMesh->MeshHead.Blocks.SubsetIndexBlock = nOffset;
	pSubset = (WORD*)(pbyData + nOffset);
	for (int i = 0; i < nFaceNum; i++)
		pSubset[i] = 0;
	nOffset += nSizeSubset;

	fwrite(pbyData, sizeof(BYTE), nOffset, pFile);
	fclose(pFile);

	return S_OK;
}

#define FLOAT4_TO_COLOR(color) (XMCOLOR((float*)&(color)))

int main()
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	XMFLOAT3 Vertices[] = {
		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		XMFLOAT3(-1.0f, +1.0f, -1.0f),
		XMFLOAT3(+1.0f, +1.0f, -1.0f),
		XMFLOAT3(+1.0f, -1.0f, -1.0f),
		XMFLOAT3(-1.0f, -1.0f, +1.0f),
		XMFLOAT3(-1.0f, +1.0f, +1.0f),
		XMFLOAT3(+1.0f, +1.0f, +1.0f),
		XMFLOAT3(+1.0f, -1.0f, +1.0f),
	};

	XMCOLOR VertexColors[] = {
		FLOAT4_TO_COLOR(Colors::White),
		FLOAT4_TO_COLOR(Colors::Black),
		FLOAT4_TO_COLOR(Colors::Red),
		FLOAT4_TO_COLOR(Colors::Green),
		FLOAT4_TO_COLOR(Colors::Blue),
		FLOAT4_TO_COLOR(Colors::Yellow),
		FLOAT4_TO_COLOR(Colors::Cyan),
		FLOAT4_TO_COLOR(Colors::Magenta),
	};

	DWORD Indices[] = {
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

	CreateMesh(Vertices, VertexColors, _countof(Vertices), Indices, _countof(Indices));

	hResult = S_OK;
Exit0:
	return hResult;
}
