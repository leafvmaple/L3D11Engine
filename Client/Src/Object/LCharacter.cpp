#include "LCharacter.h"

#include "LAssert.h"


HRESULT LCharacter::FrameMove(IL3DEngine* p3DEngine, unsigned int nFrame)
{
    return S_OK;
}

HRESULT LCharacter::Update(IL3DEngine* p3DEngine, float fDeltaTime)
{
    LState curState = m_FrameData.m_State;
    LState prevState = m_PrevFrameData.m_State;

    switch (curState)
    {
    case LState::Idle:
        if (prevState != LState::Idle)
            IdleAction();
        break;
    case LState::Forward:
        if (prevState != LState::Forward)
            ForwardAction();
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

    _UpdatePosition(p3DEngine, fDeltaTime);

    m_PrevFrameData = m_FrameData;

    return S_OK;
}

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
    PlayAnimation("data/source/player/F1/动作/F1b02yd奔跑.ani", AnimationPlayType::Circle, ANICTL_PRIMARY);
}


void LCharacter::IdleAction()
{
    PlayAnimation("data/source/player/F1/动作/F1b01ty普通待机01.ani", AnimationPlayType::Circle, ANICTL_PRIMARY);
}

void LCharacter::ProcessEvent(LEvent event)
{
}

void LCharacter::ForwardEvent()
{
    if (m_FrameData.m_State == LState::Idle)
        m_FrameData.SetState(LState::Forward);

    m_FrameData.AddPosition(1, 1, 1);
}

void LCharacter::KeyUpEvent()
{
    if (m_FrameData.m_State == LState::Forward)
        m_FrameData.SetState(LState::Idle);
}

void LCharacter::_UpdatePosition(IL3DEngine* p3DEngine, float fDeltaTime)
{

}
