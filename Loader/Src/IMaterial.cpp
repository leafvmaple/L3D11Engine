#include "IMaterial.h"
#include "LFileReader.h"

#include "rapidjson/include/rapidjson/document.h"

#include <unordered_map>

void _LoadDefine(const char* szFileName, MATERIAL_DEFINE& Define)
{
    strcpy_s(Define.szName, szFileName);

    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(szFileName, JsonDocument);

    auto& InfoObject = JsonDocument["Info"];
    strcpy_s(Define.szShaderName, InfoObject["Shader"].GetString());

    auto& ParamObjectArray = JsonDocument["Param"];
    Define.nParam = ParamObjectArray.Size();
    Define.pParam = new MATERIAL_DEFINE::_Param[ParamObjectArray.Size()];

    for (int i = 0; i < ParamObjectArray.Size(); i++)
    {
        const auto& ParamObject = ParamObjectArray[i];
        auto& Param = Define.pParam[i];
        const char* szType = ParamObject["Type"].GetString();

        strcpy_s(Param.szName, ParamObject["Name"].GetString());
        strcpy_s(Param.szRegister, ParamObject["RegisterName"].GetString());
        if (ParamObject["Value"].IsString())
            strcpy_s(Param.szValue, ParamObject["Value"].GetString());
        else if (ParamObject["Value"].IsFloat())
            Param.fValue = ParamObject["Value"].GetFloat();
    }
}

void _LoadSubset(const rapidjson::Value& JsonObject, MATERIAL_SOURCE_SUBSET& Subset)
{
    const auto& InfoObject = JsonObject["Info"];
    const char* szValue = InfoObject["RefPath"].GetString();

    _LoadDefine(szValue, Subset.Define);

    // Param
    const auto& ParamArray = JsonObject["Param"];
    Subset.pTexture = new MATERIAL_SOURCE_SUBSET::_Param[ParamArray.Size()];

    for (int i = 0; i < ParamArray.Size(); i++)
    {
        const auto& ParamObject = ParamArray[i].GetObjectW();
        const char* szType = ParamObject["Type"].GetString();

        if (!strcmp(szType, "Texture"))
        {
            auto& Texture = Subset.pTexture[Subset.nTexture];

            strcpy_s(Texture.szName, ParamObject["Name"].GetString());
            strcpy_s(Texture.szType, szType);
            strcpy_s(Texture.szValue, ParamObject["Value"].GetString());

            Subset.nTexture++;
        }
    }

    // RenderState
    const auto& RenderStateObject = JsonObject["RenderState"];
    Subset.nAlphaRef    = RenderStateObject["AlphaRef"].GetInt();
    Subset.nAlphaRef2   = RenderStateObject["AlphaRef2"].GetInt();
    Subset.nTwoSideFlag = RenderStateObject["TwoSide"].GetInt();
    Subset.nBlendMode   = RenderStateObject["BlendMode"].GetInt();
}

void LoadMaterial(MATERIAL_DESC* pDesc, MATERIAL_SOURCE*& pSource)
{
    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(pDesc->szFileName, JsonDocument);

    pSource = new MATERIAL_SOURCE;
    pSource->AddRef();

    const auto& LODArray = JsonDocument["LOD"];

    pSource->nLOD = LODArray.Size();
    pSource->pLOD = new MATERIAL_SOURCE_LOD[pSource->nLOD];
    for (int i = 0; i < LODArray.Size(); i++)
    {
        const auto& GroupArray = LODArray[i]["Group"];
        auto& LOD = pSource->pLOD[i];

        LOD.nGroup = GroupArray.Size();
        LOD.pGroup = new MATERIAL_SOURCE_GROUP[LOD.nGroup];
        for (int j = 0; j < GroupArray.Size(); j++)
        {
            const auto& SubsetArray = GroupArray[j]["Subset"];
            auto& Group = LOD.pGroup[j];

            Group.nSubset = SubsetArray.Size();
            Group.pSubset = new MATERIAL_SOURCE_SUBSET[Group.nSubset];
            for (int k = 0; k < SubsetArray.Size(); k++)
            {
                _LoadSubset(SubsetArray[k], Group.pSubset[k]);
            }
        }
    }
}