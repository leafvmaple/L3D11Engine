#include "LCharacter.h"

#include "LAssert.h"

HRESULT LCharacter::Create(IL3DEngine* p3DEngine, const char* szPath)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    hr = ILModel::Create(p3DEngine, szPath, &m_pObject);
    HRESULT_ERROR_EXIT(hr);

    m_p3DEngine = p3DEngine;

    LoadPart("data/source/player/F1/部件/F1_0000_head.mesh");
    LoadPart("data/source/player/F1/部件/F1_2206_body.mesh");
    LoadPart("data/source/player/F1/部件/F1_2206_hand.mesh");
    LoadPart("data/source/player/F1/部件/F1_2206_leg.mesh");
    LoadPart("data/source/player/F1/部件/F1_2206_belt.mesh");
    LoadSocket("data/source/player/F1/部件/f1_new_face.Mesh", "s_face");
    LoadSocket("data/source/player/F1/部件/F1_2206_hat.mesh", "s_hat");
    LoadSocket("data/source/item/weapon/brush/RH_brush_001.Mesh", "s_rh");

    IdleAction();

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


void LCharacter::ForwardAction()
{
    m_State = LState::Forward;
    PlayAnimation("data/source/player/F1/动作/F1b02yd奔跑.ani", AnimationPlayType::Circle, ANICTL_PRIMARY);
}


void LCharacter::IdleAction()
{
    m_State = LState::Idle;
    PlayAnimation("data/source/player/F1/动作/F1b01ty普通待机01.ani", AnimationPlayType::Circle, ANICTL_PRIMARY);
}

void LCharacter::ProcessEvent(LEvent event)
{
    switch (m_State)
    {
    case LState::Idle:
        if (event == LEvent::Forward)
            ForwardAction();
        break;
    case LState::Forward:
        if (event == LEvent::KeyUp)
            IdleAction();
        break;
    case LState::Backward:
        break;
    case LState::Left:
        break;
    case LState::Right:
        break;
    case LState::Jump:
        break;
    case LState::Attack:
        break;
    case LState::Skill:
        break;
    case LState::Dead:
        break;
    default:
        break;
    }
}

void LCharacter::ForwardEvent()
{
    ProcessEvent(LEvent::Forward);
}

void LCharacter::KeyUpEvent()
{
    ProcessEvent(LEvent::KeyUp);
}
