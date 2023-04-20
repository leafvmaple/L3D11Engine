#include "stdafx.h"
#include "L3DScene.h"

#include "L3DFormat.h"
#include "L3DEnvironment.h"

#include "Model/L3DModel.h"
#include "State/L3DState.h"
#include "Render/L3DRenderUnit.h"

#include "IO/LFileReader.h"
#include "Utility/FilePath.h"

HRESULT L3DScene::Create(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    L3D_SCENE_RECT rect;
    SCENE_PATH_TABLE pathTable;

    hr = _CreatePathTable(szFileName, &pathTable);
    HRESULT_ERROR_EXIT(hr);

    hr = _LoadLogicalSceneRect(pathTable.szSetting, &rect);
    HRESULT_ERROR_EXIT(hr);

    hr = _LoadEnvironmentSetting(piDevice, pathTable);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DScene::SetParent(IL3DEngine* piEngine)
{
    m_piEngine = piEngine;
    return S_OK;
}

void L3DScene::BeginRender(const SCENE_RENDER_OPTION& RenderOption)
{
    _UpdateVisibleList();
    _UpdateCommonRenderData(RenderOption);

    return;
}

void L3DScene::EndRender(const SCENE_RENDER_OPTION& RenderOption)
{
    _RenderMainCamera(RenderOption);
}

HRESULT L3DScene::AddRenderEntity(ILModel* piModel)
{
    L3DModel* pModel = dynamic_cast<L3DModel*>(piModel);
    BOOL_ERROR_EXIT(pModel);

    m_DynamicalObjects.push_back(pModel);

Exit0:
    return S_OK;
}

void L3DScene::GetVisibleObjectAll(MODEL_VECTOR& vecModels)
{
    vecModels = m_DynamicalObjects;
}

HRESULT L3DScene::_CreatePathTable(const char* szFileName, SCENE_PATH_TABLE* pPathTable)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    int nRetCode = false;

    char szMapName[MAX_PATH];

    memset(pPathTable, 0, sizeof(SCENE_PATH_TABLE));

    strcpy(pPathTable->szSetting, szFileName);

    hr = L3D::FormatExtName(pPathTable->szSetting, "_Setting.ini");
    HRESULT_ERROR_EXIT(hr);

    hr = L3D::PathSplit(szFileName, pPathTable->szDir, MAX_PATH, szMapName, MAX_PATH);
    HRESULT_ERROR_EXIT(hr);

    nRetCode = sprintf_s(pPathTable->szSceneInfo, "%hsentities/%hs_sceneinfo.json", pPathTable->szDir, szMapName);
    BOOL_ERROR_EXIT(nRetCode > 0);

    nRetCode = sprintf_s(pPathTable->szEnvironment, "%senvironment.json", pPathTable->szDir);
    BOOL_ERROR_EXIT(nRetCode > 0);

    nRetCode = sprintf_s(pPathTable->szCoverMap, "%s%s.tga", pPathTable->szDir, szMapName);
    BOOL_ERROR_EXIT(nRetCode > 0);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DScene::_LoadLogicalSceneRect(const char* szSettingName, L3D_SCENE_RECT* pRect)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    CSimpleIniA Ini;

    hr = LFileReader::ReadIni(szSettingName, Ini);
    HRESULT_ERROR_EXIT(hr);

    pRect->fXStart = Ini.GetDoubleValue("LogicalScene", "X_Start");
    pRect->fZStart = Ini.GetDoubleValue("LogicalScene", "Z_Start");
    pRect->fXWidth = Ini.GetDoubleValue("LogicalScene", "X_Width");
    pRect->fZWidth = Ini.GetDoubleValue("LogicalScene", "Z_Width");

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DScene::_LoadEnvironmentSetting(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable)
{
    m_pEnvironment = new L3DEnvironment;

    m_pEnvironment->Load(piDevice, pathTable);


    return S_OK;
}

void L3DScene::_UpdateVisibleList()
{
    GetVisibleObjectAll(m_VisibleModel);

    // Cull In This
}

void L3DScene::_UpdateCommonConstData(const SCENE_RENDER_OPTION& RenderOption)
{
    RenderOption.piImmediateContext->RSSetState(RenderOption.pStateTable->Rasterizer[L3D_RASTERIZER_STATE_CULL_CLOCKWISE]);
}

void L3DScene::_UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption)
{
    for (auto& pModel : m_VisibleModel)
        pModel->UpdateCommonRenderData(RenderOption);
}

void L3DScene::_RenderMainCamera(const SCENE_RENDER_OPTION& RenderOption)
{
    m_RenderQueues.GBuffer.clear();

    for (auto& pModel : m_VisibleModel)
        pModel->GetRenderUnit(m_RenderQueues.GBuffer);

    _UpdateCommonConstData(RenderOption);

    for (auto& pRenderUnit : m_RenderQueues.GBuffer)
        pRenderUnit->ApplyProcess(RenderOption, RENDER_PASS::COLOR);
}
