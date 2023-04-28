#include "LObjectMgr.h"
#include "LAssert.h"
#include "Object/LModel.h"
#include "World/LScene.h"

LObjectMgr::LObjectMgr()
{
    m_ObjectList.clear();
}

LObjectMgr::~LObjectMgr()
{
    for (auto& object : m_ObjectList)
        SAFE_DELETE(object);
    
    for (auto& scene : m_SceneList)
        SAFE_DELETE(scene);
}

HRESULT LObjectMgr::Init(HINSTANCE hInstance, L3D_WINDOW_PARAM& WindowParam)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    hr = IL3DEngine::Instance()->Init(hInstance, WindowParam);
    HRESULT_ERROR_EXIT(hr);

Exit0:
    return S_OK;
}

void LObjectMgr::Uninit()
{
    IL3DEngine::Instance()->Uninit();
}


LObjectMgr& LObjectMgr::Instance()
{
    static LObjectMgr ObjectMgr;
    return ObjectMgr;
}

LScene* LObjectMgr::CreateScene(const char* szFileName)
{
    LScene* pScene = new LScene;

    pScene->Create(szFileName);
    m_SceneList.emplace_back(pScene);

    return pScene;
}

HRESULT LObjectMgr::Update(float fDeltaTime)
{
    IL3DEngine* piEngine = IL3DEngine::Instance();

    for (auto& object : m_ObjectList)
        object->Display(piEngine, fDeltaTime);

    for (auto& scene : m_SceneList)
        scene->Update(fDeltaTime);

    piEngine->Update(fDeltaTime);

    return S_OK;
}

BOOL LObjectMgr::IsActive()
{
    return IL3DEngine::Instance()->IsActive();
}