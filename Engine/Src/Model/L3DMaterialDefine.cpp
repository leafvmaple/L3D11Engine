#include "stdafx.h"
#include "L3DMaterialDefine.h"

#include "L3DTexture.h"

#include "IO/LFileReader.h"
#include "Utility/FilePath.h"

HRESULT L3DMaterialDefine::Create(const char* szFileName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    rapidjson::Document JsonDocument;

    strcpy(m_szName, szFileName);

    hr = LFileReader::ReadJson(szFileName, JsonDocument);
    HRESULT_ERROR_EXIT(hr);

    {
        auto& InfoObject = JsonDocument["Info"];
        strcpy(m_szShaderName, InfoObject["Shader"].GetString());

        auto& ParamObjectArray = JsonDocument["Param"];
        for (auto iter = ParamObjectArray.Begin(), iend = ParamObjectArray.End(); iter != iend; ++iter)
        {
            auto ParamObject = iter->GetObject();
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

    hr = L3D::ReplaceExtName(m_szShaderName, ".fx5");
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
