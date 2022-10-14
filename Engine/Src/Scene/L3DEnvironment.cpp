#include "stdafx.h"

#include "L3DFormat.h"
#include "L3DEnvironment.h"

#include "Model/L3DTexture.h"

#define DEFAULT_WHITE_TEXTURE	 "Res/public/defaultWhite.dds"

HRESULT L3DEnvironment::Load(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable)
{
	HRESULT hr = E_FAIL;

	CSimpleIniA Ini;

	ENVIRONMENT_FILE_DATA envData;

	Ini.LoadFile(pathTable.szSetting);

	hr = _LoadDefault(&envData);
	HRESULT_ERROR_EXIT(hr);

	hr = _LoadFromIni(Ini, &envData);
	HRESULT_ERROR_EXIT(hr);

	hr = _LoadCoverMap(piDevice, pathTable.szCoverMap);
	HRESULT_ERROR_EXIT(hr);

Exit0:
	return S_OK;
}

HRESULT L3DEnvironment::_LoadDefault(ENVIRONMENT_FILE_DATA* pData)
{
	pData->ConverMapDesc.sConverMap = DEFAULT_WHITE_TEXTURE;

	return S_OK;
}

HRESULT L3DEnvironment::_LoadFromIni(const CSimpleIniA& Ini, ENVIRONMENT_FILE_DATA* pData)
{
	pData->ConverMapDesc.sConverMap = Ini.GetValue("ConverMap", "ConverMapFilePath", "data/public/convermap.tga");

	return S_OK;
}

HRESULT L3DEnvironment::_LoadCoverMap(ID3D11Device* piDevice, const char* szConvertMap)
{
	HRESULT hr = E_FAIL;

	m_sConverMap = szConvertMap;

	m_piConverMap = new L3DTexture;
	BOOL_ERROR_EXIT(m_piConverMap);

	hr = m_piConverMap->Create(piDevice, szConvertMap);
	HRESULT_ERROR_EXIT(hr);

Exit0:
	return S_OK;
}

