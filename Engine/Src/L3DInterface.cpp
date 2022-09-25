#include "stdafx.h"

#include "L3DInterface.h"
#include "L3DEngine.h"

#include "Model/L3DModel.h"

IL3DEngine* IL3DEngine::m_pInstance = NULL;

IL3DEngine* IL3DEngine::Instance()
{
    if (!m_pInstance)
        m_pInstance = new L3DEngine;
    return m_pInstance;
}

HRESULT ILModel::Create(IL3DEngine* pL3DEngine, const char* szFileName, ILModel** ppModel)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    L3DEngine* pEngine = NULL;
    L3DModel* pModel = NULL;

    BOOL_ERROR_EXIT(pL3DEngine);

    pEngine = dynamic_cast<L3DEngine*>(pL3DEngine);
    BOOL_ERROR_EXIT(pEngine);

    pModel = new L3DModel;
    BOOL_ERROR_EXIT(pModel);

    hr = pModel->Create(pL3DEngine->GetDevice(), szFileName);
    HRESULT_ERROR_EXIT(hr);

    pEngine->AttachObject(pModel);
    *ppModel = pModel;

    hResult = S_OK;

Exit0:
    return hResult;
}
