#include "LCharacter.h"

#include "LAssert.h"

HRESULT LCharacter::Create(IL3DEngine* p3DEngine, const char* szPath)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    hr = ILModel::Create(p3DEngine, szPath, &m_pObject);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
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


HRESULT LCharacter::PlayAnimation(const char* szAnimation, ANIMATION_PLAY_TYPE nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    return m_pObject->PlayAnimation(szAnimation, nPlayType, nPriority);
}

void LCharacter::AppendRenderEntity(ILScene* piScene)
{ 
    piScene->AddRenderEntity(m_pObject);
}
