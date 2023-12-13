#include "stdafx.h"
#include "L3DScene.h"

#include "L3DFormat.h"
#include "L3DEnvironment.h"
#include "L3DLandscape.h"

#include "Model/L3DModel.h"
#include "Camera/L3DCamera.h"
#include "State/L3DState.h"
#include "Render/L3DRenderUnit.h"
#include "Render/L3DMaterialSystem.h"

#include "IO/LFileReader.h"
#include "Utility/FilePath.h"

HRESULT L3DScene::Create(ID3D11Device* piDevice, const char* szFileName)
{
    L3D_SCENE_RECT rect;
    SCENE_PATH_TABLE pathTable;

    _CreatePathTable(szFileName, &pathTable);
    _CreateCamera();

    _LoadLogicalSceneRect(pathTable.setting.c_str(), &rect);
    _LoadEnvironmentSetting(piDevice, pathTable);

    _LoadLandscape(piDevice, pathTable);

    return S_OK;
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
    m_pLandscape->UpdateVisibility();
    _UpdateCommonTextures(RenderOption);

    return;
}

void L3DScene::EndRender(const SCENE_RENDER_OPTION& RenderOption)
{
    _RenderMainCamera(RenderOption);
}

void L3DScene::Update(const SCENE_RENDER_OPTION& RenderOption)
{
    for (auto& object : m_DynamicalObjects)
        object->PrimaryUpdate();
}

void L3DScene::AddRenderEntity(ILModel* piModel)
{
    L3DModel* pModel = dynamic_cast<L3DModel*>(piModel);
    m_DynamicalObjects.push_back(pModel);
}

void L3DScene::GetVisibleObjectAll(MODEL_VECTOR& vecModels)
{
    for (const auto& dynamic : m_DynamicalObjects)
        dynamic->GetAllModel(vecModels);
}

void L3DScene::UpdateCamera()
{
    m_RenderContext.RenderToTargeParam.CameraInfo = m_pCamera->GetCameraInfo();
}

ILCamera* L3DScene::Get3DCamera()
{
    return m_pCamera;
}

void L3DScene::_CreateCamera()
{
    m_pCamera = new(std::nothrow) L3DCamera;
    m_pCamera->Init();
}

HRESULT L3DScene::_CreatePathTable(const char* szFileName, SCENE_PATH_TABLE* pPathTable)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    int nRetCode = false;

    pPathTable->dir = std::filesystem::path(szFileName).parent_path();
    pPathTable->mapName = pPathTable->dir.stem();

    pPathTable->environment = pPathTable->dir / "environment.json";
    pPathTable->coverMap = pPathTable->dir / (pPathTable->mapName.string() + ".tga");
    pPathTable->setting = pPathTable->dir / (pPathTable->mapName.string() + "_Setting.ini");
    pPathTable->landscape = pPathTable->dir / "landscape" / (pPathTable->mapName.string() + "_landscapeinfo.json");

    pPathTable->dir = pPathTable->dir.lexically_normal();
    pPathTable->environment = pPathTable->environment.lexically_normal();
    pPathTable->coverMap = pPathTable->coverMap.lexically_normal();
    pPathTable->setting = pPathTable->setting.lexically_normal();
    pPathTable->landscape = pPathTable->landscape.lexically_normal();

    hResult = S_OK;
Exit0:
    return hResult;
}

void L3DScene::_CreateScene(const char* szFileName)
{

}

HRESULT L3DScene::_LoadLogicalSceneRect(const wchar_t* szSettingName, L3D_SCENE_RECT* pRect)
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

HRESULT L3DScene::_LoadLandscape(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable)
{
    m_pLandscape = new L3DLandscape;

    m_pLandscape->Load(piDevice, pathTable);

    return S_OK;
}

void L3DScene::_UpdateVisibleList()
{
    m_VisibleModel.clear();
    GetVisibleObjectAll(m_VisibleModel);

    // Cull In This
}

void L3DScene::_UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption)
{
    for (auto& pModel : m_VisibleModel)
        pModel->UpdateCommonRenderData(RenderOption);
}

void L3DScene::_UpdateCommonTextures(const SCENE_RENDER_OPTION& RenderOption)
{
    ID3D11SamplerState* Samplers[MTLSYS_COMMON_SPL_COUNT] = {
        RenderOption.pStateTable->Sampler[L3D_SAMPLER_STATE_POINT_CLAMP]
    };

    g_pMaterialSystem->SetCommonShaderSamples(RenderOption.piImmediateContext, Samplers);
}

void L3DScene::_CommitConstData(const SCENE_RENDER_OPTION& RenderOption)
{
    m_ShaderCommonParam.Camera.CameraView = m_RenderContext.RenderToTargeParam.CameraInfo.mView;
    m_ShaderCommonParam.Camera.CameraProject = m_RenderContext.RenderToTargeParam.CameraInfo.mProject;

    g_pMaterialSystem->SetCommonShaderData(RenderOption.piImmediateContext, m_ShaderCommonParam);
}

void L3DScene::_UpdateCommonConstData(const SCENE_RENDER_OPTION& RenderOption)
{
    _CommitConstData(RenderOption);
}

void L3DScene::_RenderMainCamera(const SCENE_RENDER_OPTION& RenderOption)
{
    m_RenderQueues.GBufferQueue.clear();

    for (auto& pModel : m_VisibleModel)
        pModel->GetRenderUnit(m_RenderQueues);

    _UpdateCommonConstData(RenderOption);

    for (auto& pRenderUnit : m_RenderQueues.GBufferQueue)
        pRenderUnit->ApplyProcess(RenderOption, RENDER_PASS::COLOR);

    m_pLandscape->RenderTerrain(RenderOption, RENDER_PASS::COLOR);
}
