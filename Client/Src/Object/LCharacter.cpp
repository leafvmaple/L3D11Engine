#include "LCharacter.h"

#include "LAssert.h"

HRESULT LCharacter::Create(IL3DEngine* p3DEngine, const char* szPath)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    hr = ILModel::Create(p3DEngine, szPath, &m_pObject);
    HRESULT_ERROR_EXIT(hr);

    m_p3DEngine = p3DEngine;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT LCharacter::LoadPart(const char* szPath)
{
    ILModel* piModel = nullptr;

    ILModel::Create(m_p3DEngine, szPath, &piModel);
    m_Parts.emplace_back(piModel);

    m_pObject->AttachModel(piModel);

    return S_OK;
}

HRESULT LCharacter::LoadSocket(const char* szPath, const char* szSocketName)
{
    ILModel* piModel = nullptr;

    ILModel::Create(m_p3DEngine, szPath, &piModel);
    m_Parts.emplace_back(piModel);

    piModel->BindToSocket(m_pObject, szSocketName);

    return S_OK;
}

HRESULT LCharacter::SetPosition(const XMFLOAT3& Position)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    BOOL_ERROR_EXIT(m_pObject);

    hr = m_pObject->SetTranslation(Position);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT LCharacter::PlayAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    return m_pObject->PlayAnimation(szAnimation, nPlayType, nPriority);
}

void LCharacter::AppendRenderEntity(ILScene* piScene)
{ 
    piScene->AddRenderEntity(m_pObject);
}

void LCharacter::Run()
{
    m_StateMachine.process_event(LEvent::Run{});
}

void LCharacter::Stand()
{
    m_StateMachine.process_event(LEvent::Stop{});
}

