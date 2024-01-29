#include "stdafx.h"

#include "L3DInterface.h"
#include "L3DEngine.h"

#include "Model/L3DMesh.h"
#include "Model/L3DSkeleton.h"
#include "Model/L3DAnimation.h"
#include "Model/L3DModel.h"

#include "Flexible/L3DFlexible.h"

#include "Scene/L3DScene.h"

IL3DEngine* IL3DEngine::m_pInstance = nullptr;

IL3DEngine* IL3DEngine::Instance()
{
    if (!m_pInstance)
        m_pInstance = new L3DEngine;
    return m_pInstance;
}

void IL3DEngine::Destroy()
{
    SAFE_DELETE(m_pInstance);
}

HRESULT ILModel::Create(IL3DEngine* pL3DEngine, const char* szFileName, ILModel** ppModel)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    L3DEngine* pEngine = nullptr;
    L3DModel* pModel = nullptr;

    BOOL_ERROR_EXIT(pL3DEngine);

    pEngine = dynamic_cast<L3DEngine*>(pL3DEngine);
    BOOL_ERROR_EXIT(pEngine);

    pModel = new L3DModel;
    BOOL_ERROR_EXIT(pModel);

    pModel->Create(pL3DEngine->GetDevice(), szFileName);

    *ppModel = pModel;

    hResult = S_OK;

Exit0:
    return hResult;
}

HRESULT ILScene::Create(IL3DEngine* pL3DEngine, const char* szFileName, ILScene** ppScene)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	L3DEngine* pEngine = nullptr;
	L3DScene* pScene = nullptr;

	BOOL_ERROR_EXIT(pL3DEngine);

	pEngine = dynamic_cast<L3DEngine*>(pL3DEngine);
	BOOL_ERROR_EXIT(pEngine);

    pScene = new L3DScene;
    BOOL_ERROR_EXIT(pScene);

    hr = pScene->SetParent(pL3DEngine);
    HRESULT_ERROR_EXIT(hr);

    hr = pScene->Create(pL3DEngine->GetDevice(), szFileName);
    HRESULT_ERROR_EXIT(hr);

    pEngine->AttachScene(pScene);

    *ppScene = pScene;

    hResult = S_OK;
Exit0:
    return hResult;
}

