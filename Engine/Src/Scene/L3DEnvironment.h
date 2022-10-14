#pragma once

#include <string>

#include "simpleini/SimpleIni.h"

struct SCENE_PATH_TABLE;

class L3DTexture;

class L3DEnvironment
{
public:
	HRESULT Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);

private:
	struct CONVERMAP_DESC
	{
		XMFLOAT4 Rect;
		float fLowest;
		float fHighest;
		std::string sConverMap;
	};

	struct ENVIRONMENT_FILE_DATA
	{
		CONVERMAP_DESC ConverMapDesc;
	};

	HRESULT _LoadDefault(ENVIRONMENT_FILE_DATA* pData);
	HRESULT _LoadFromIni(const CSimpleIniA& Ini, ENVIRONMENT_FILE_DATA* pData);
	HRESULT _LoadCoverMap(ID3D11Device* piDevice, const char* szConvertMap);

	std::string m_sConverMap;
	L3DTexture* m_piConverMap = nullptr;
};