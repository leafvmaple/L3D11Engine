#include "LObjectMgr.h"
#include "LAssert.h"
#include "Object/LModel.h"

LObjectMgr::LObjectMgr()
{
    m_ObjectList.clear();
}

LObjectMgr::~LObjectMgr()
{
    LModel* pObject = NULL;
    std::list<LModel*>::iterator it;

    for (it = m_ObjectList.begin(); it != m_ObjectList.end(); it++)
    {
        pObject = *it;
        SAFE_DELETE(pObject);
    }
}

HRESULT LObjectMgr::Init(HINSTANCE hInstance, L3DWINDOWPARAM& WindowParam)
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

HRESULT LObjectMgr::Update(float fDeltaTime)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    LModel* pObject = NULL;
    std::list<LModel*>::iterator it;

    for (it = m_ObjectList.begin(); it != m_ObjectList.end(); it++)
    {
        pObject = *it;
        BOOL_ERROR_CONTINUE(pObject);
        pObject->Display(IL3DEngine::Instance(), fDeltaTime);
    }

    hr = IL3DEngine::Instance()->Update(fDeltaTime);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

BOOL LObjectMgr::IsActive()
{
    return IL3DEngine::Instance()->IsActive();
}