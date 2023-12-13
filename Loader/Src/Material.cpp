#include "Material.h"
#include "LFileReader.h"

#include <unordered_map>

void _LoadDefine(const char* szFileName, MATERIAL_DEFINE& Define)
{
    strcpy_s(Define.szName, szFileName);

    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(szFileName, JsonDocument);

    auto& InfoObject = JsonDocument["Info"];
    RapidJsonGet<MAX_PATH>(Define.szShaderName, InfoObject, "Shader");

    auto& ParamObjectArray = JsonDocument["Param"];
    Define.nParam = ParamObjectArray.Size();
    Define.pParam = new MATERIAL_DEFINE::_Param[ParamObjectArray.Size()];

    for (int i = 0; i < ParamObjectArray.Size(); i++)
    {
        const auto& ParamObject = ParamObjectArray[i];
        auto& Param = Define.pParam[i];

        RapidJsonGet<MAX_PATH>(Param.szName, ParamObject, "Name");
        RapidJsonGet<MAX_PATH>(Param.szRegister, ParamObject, "RegisterName");
        RapidJsonGet<MAX_PATH>(Param.szValue, ParamObject, "Value");
        RapidJsonGet(Param.fValue, ParamObject, "Value");
    }
}

void _LoadMaterial(const rapidjson::Value& JsonObject, MATERIAL_SOURCE& Subset)
{
    const auto& InfoObject = JsonObject["Info"];
    const char* szValue = InfoObject["RefPath"].GetString();

    _LoadDefine(szValue, Subset.Define);

    // Param
    const auto& ParamArray = JsonObject["Param"];
    Subset.pTexture = new MATERIAL_SOURCE::_Param[ParamArray.Size()];

    for (int i = 0; i < ParamArray.Size(); i++)
    {
        const auto& ParamObject = ParamArray[i];
        const char* szType = ParamObject["Type"].GetString();

        if (!strcmp(szType, "Texture"))
        {
            auto& Texture = Subset.pTexture[Subset.nTexture];

            RapidJsonGet<MAX_PATH>(Texture.szName, ParamObject, "Name");
            RapidJsonGet<MAX_PATH>(Texture.szValue, ParamObject, "Value");

            strcpy(Texture.szType, szType);

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

void LoadModelMaterial(MODEL_MATERIAL_DESC* pDesc, MODEL_MATERIAL_SOURCE*& pSource)
{
    rapidjson::Document JsonDocument;
    LFileReader::ReadJson(pDesc->szFileName, JsonDocument);

    pSource = new MODEL_MATERIAL_SOURCE;
    pSource->AddRef();

    const auto& LODArray = JsonDocument["LOD"];

    pSource->nLOD = LODArray.Size();
    pSource->pLOD = new MATERIAL_LOD_SOURCE[pSource->nLOD];
    for (int i = 0; i < LODArray.Size(); i++)
    {
        const auto& GroupArray = LODArray[i]["Group"];
        auto& LOD = pSource->pLOD[i];

        LOD.nGroup = GroupArray.Size();
        LOD.pGroup = new MATERIAL_GROUP_SOURCE[LOD.nGroup];
        for (int j = 0; j < GroupArray.Size(); j++)
        {
            const auto& SubsetArray = GroupArray[j]["Subset"];
            auto& Group = LOD.pGroup[j];

            Group.nSubset = SubsetArray.Size();
            Group.pSubset = new MATERIAL_SOURCE[Group.nSubset];
            for (int k = 0; k < SubsetArray.Size(); k++)
            {
                _LoadMaterial(SubsetArray[k], Group.pSubset[k]);
            }
        }
    }
}