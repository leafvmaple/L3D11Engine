#include "stdafx.h"

#include "L3DModel.h"
#include "L3DMesh.h"
#include "L3DMaterial.h"
#include "L3DTexture.h"

#include "IO/LFileReader.h"
#include "LCommon.h"

#include "Render/L3DRenderUnit.h"

HRESULT L3DModel::Create(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

	char szMaterialName[MAX_PATH];

	m_pMesh = new L3DMesh;
	BOOL_ERROR_EXIT(m_pMesh);

	hr = m_pMesh->Create(piDevice, szFileName);
	HRESULT_ERROR_EXIT(hr);

    strcpy(szMaterialName, szFileName);
    hr = L3D::FormatExtName(szMaterialName, ".JsonInspack");
	if (SUCCEEDED(hr))
	{
		hr = _LoadMaterialFromJson(piDevice, szMaterialName);
		HRESULT_ERROR_EXIT(hr);
	}

    m_pRenderUnit = new L3DRenderUnit;
    BOOL_ERROR_EXIT(m_pRenderUnit);

    hr = m_pRenderUnit->Create(m_pMesh->m_Stage, m_vecMaterial[0]);
    HRESULT_ERROR_EXIT(hr);

    hr = ResetTransform();
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT L3DModel::ResetTransform()
{
	XMStoreFloat4x4(&m_World, XMMatrixIdentity());
	XMStoreFloat4(&m_Rotation, g_XMIdentityR3);
	XMStoreFloat3(&m_Scale, g_XMOne);
	XMStoreFloat3(&m_Translation, g_XMZero);

    m_pRenderUnit->SetWorldMatrix(m_World);

    return S_OK;
}

HRESULT L3DModel::SetTranslation(const XMFLOAT3& Translation)
{
    m_Translation = Translation;

    UpdateTransFrom();

    return S_OK;
}

HRESULT L3DModel::SetRotation(const XMFLOAT4& Rotation)
{
    m_Rotation = Rotation;

    UpdateTransFrom();

    return S_OK;
}

HRESULT L3DModel::SetScale(const XMFLOAT3& Scale)
{
    m_Scale = Scale;

    UpdateTransFrom();

    return S_OK;
}

void L3DModel::UpdateTransFrom()
{
    XMStoreFloat4x4(&m_World,
        XMMatrixTransformation(
            g_XMZero,
            g_XMIdentityR3,
            XMLoadFloat3(&m_Scale),
            g_XMZero,
            XMLoadFloat4(&m_Rotation),
            XMLoadFloat3(&m_Translation)
        )
    );

    m_pRenderUnit->SetWorldMatrix(m_World);
}

HRESULT L3DModel::_LoadMaterialFromJson(ID3D11Device* piDevice, const char* szFileName)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	rapidjson::Document JsonDocument;

	hr = LFileReader::ReadJson(szFileName, JsonDocument);
	HRESULT_ERROR_EXIT(hr);

	{
		auto& LODArray = JsonDocument["LOD"];

		unsigned uLODCount = LODArray.Capacity();

		for (int i = 0; i < uLODCount; i++)
		{
			auto& GroupArray = LODArray[i]["Group"];
			unsigned uGroupCount = GroupArray.Capacity();

			for (int j = 0; j < uGroupCount; j++)
			{
				auto& SubsetArray = GroupArray[j]["Subset"];
				unsigned uSubsetCount = SubsetArray.Capacity();

				for (int k = 0; k < uSubsetCount; k++)
				{
					auto& SubsetObject = SubsetArray[k];
					MATERIAL_INSTANCE_DATA InstanceData;

					hr = _LoadInstanceFromJson(SubsetObject, InstanceData);
					HRESULT_ERROR_EXIT(hr);

					if (m_pMesh->m_dwBoneCount > 0)
						InstanceData.eMacro = RUNTIME_MACRO_SKIN_MESH;

					hr = _LoadSubsetMaterial(piDevice, InstanceData);
					HRESULT_ERROR_EXIT(hr);
				}
			}
		}
	}

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT L3DModel::_LoadInstanceFromJson(rapidjson::Value& JsonObject, MATERIAL_INSTANCE_DATA& InstanceData)
{
	auto& InfoObject = JsonObject["Info"];

	LPCSTR szValue = InfoObject["RefPath"].GetString();
	strcpy_s(InstanceData.szDefineName, szValue);

	auto& ParamArray = JsonObject["Param"];
	for (auto iter = ParamArray.Begin(); iter != ParamArray.End(); ++iter)
	{
		auto ParamObject = iter->GetObjectW();

		LPCSTR szName = ParamObject["Name"].GetString();
		std::string szType = ParamObject["Type"].GetString();

		if (szType == "Texture")
		{
			szValue = ParamObject["Value"].GetString();
			InstanceData.TextureArray.insert(std::make_pair(szName, szValue));
		}
	}

	return S_OK;
}

HRESULT L3DModel::_LoadSubsetMaterial(ID3D11Device* piDevice, MATERIAL_INSTANCE_DATA& InstanceData)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	L3DMaterial* pMaterial = nullptr;

	pMaterial = new L3DMaterial;
	BOOL_ERROR_EXIT(pMaterial);

	hr = pMaterial->Create(piDevice, InstanceData);
	HRESULT_ERROR_EXIT(hr);

	m_vecMaterial.push_back(pMaterial);

	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
		SAFE_DELETE(pMaterial);
	return hResult;
}