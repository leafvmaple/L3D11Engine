#include "stdafx.h"
#include "L3DMaterialDefine.h"

#include "L3DTexture.h"

#include "IO/LFileReader.h"
#include "LCommon.h"

HRESULT L3DMaterialDefine::Create(const char* szFileName)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	rapidjson::Document JsonDocument;
	size_t uSize = 0;
	BYTE* pData = nullptr;

	strcpy(m_szName, szFileName);

	hr = LFileReader::ReadJson(szFileName, JsonDocument);
	HRESULT_ERROR_EXIT(hr);

	{
		auto& InfoObject = JsonDocument["Info"];
		strcpy(m_szShaderName, InfoObject["Shader"].GetString());

		auto& ParamObjectArray = JsonDocument["Param"];
		for (auto iter = ParamObjectArray.Begin(), iend = ParamObjectArray.End(); iter != iend; ++iter)
		{
			auto ParamObject = iter->GetObjectW();
			std::string sType = ParamObject["Type"].GetString();

			if (sType == "Texture")
			{
				m_vecTexture.push_back({
					ParamObject["Name"].GetString(),
					ParamObject["RegisterName"].GetString(),
					ParamObject["Value"].GetString(),
					});
			}
		}
	}


	hr = L3D::FormatExtName(m_szShaderName, ".fx5");
	HRESULT_ERROR_EXIT(hr);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT L3DMaterialDefine::GetTextureVariables(ID3D11Device* piDevice, std::vector<TEXTURE_DATA>& Variables)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	Variables.clear();
	Variables.reserve(m_vecTexture.size());

	for (auto iter = m_vecTexture.begin(); iter != m_vecTexture.end(); ++iter)
	{
		L3DTexture* pTexture = new L3DTexture;
		BOOL_ERROR_EXIT(pTexture);

		hr = pTexture->Create(piDevice, iter->tValue.c_str());
		if (FAILED(hr))
			SAFE_DELETE(pTexture);

		Variables.push_back({ iter->hsRepresentName, iter->hsRegisterName, pTexture });
	}

	hResult = S_OK;
Exit0:
	return hResult;
}
