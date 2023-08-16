#pragma once

#include <d3d11.h>
#include <string>
#include <vector>
#include <memory>

class L3DMaterial;

struct MaterialTextureTable
{
	std::string hsRepresentName;
	std::string hsRegisterName;
	std::string tValue;
};

struct TEXTURE_DATA;

class L3DMaterialDefine
{
public:
	HRESULT Create(const char* szFileName);
	HRESULT GetTextureVariables(ID3D11Device* piDevice, std::vector<TEXTURE_DATA>& Variables);

	char m_szShaderName[256] = {0};
private:
	char m_szName[256] = {0};

	std::vector<MaterialTextureTable> m_vecTexture;
};

typedef std::vector<std::vector<std::vector<L3DMaterial>>> MATERIALS_PACK;