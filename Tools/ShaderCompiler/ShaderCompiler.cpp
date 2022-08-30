// ShaderCompiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <d3d11.h>
#include <d3dcompiler.h>
#include <cstdio>

#include "LAssert.h"

#pragma comment(lib, "D3DCompiler.lib")

int main()
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	DWORD dwShaderFlags = 0;
	size_t uSize = 0;
	size_t uHeaderSize = sizeof(GUID);
	ID3DBlob* pCompiledShader = 0;
	ID3DBlob* pCompilationMsgs = 0;
	BYTE* pbyShader = nullptr;
	FILE* f = nullptr;

	fopen_s(&f, "../../Client/Res/Shader/color.fxo", "rb");

	fseek(f, 0, SEEK_END);
	uSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	pbyShader = new BYTE[uHeaderSize + uSize];
	memset(pbyShader, 0, uHeaderSize);

	fread(pbyShader, sizeof(BYTE), uSize, f);

	fclose(f);

	fopen_s(&f, "../../Client/Res/Shader/Base/FXO/g_vs_VERTEX_SHADER_BOX_ASSEMBLY.fxo", "wb");
	
	fwrite(pbyShader, sizeof(BYTE), uHeaderSize + uSize, f);

	hResult = S_OK;
Exit0:
	return hResult;
}
