#include "LCharacter.h"

#include "LAssert.h"

// const int MOVE_DEST_RANGE = 1 << 12;

const int MOVE_DEST_RANGE = 16;

HRESULT LCharacter::FrameMove(IL3DEngine* p3DEngine, unsigned int nFrame)
{
    if (m_MoveCtrl.bMove)
    {
        m_MovePosition.nX += MOVE_DEST_RANGE;
        m_MovePosition.nY += MOVE_DEST_RANGE;
    }

    return S_OK;
}

HRESULT LCharacter::Update(IL3DEngine* p3DEngine, float fDeltaTime)
{
    if (m_MovePosition.State != m_PrevMovePosition.State)
    {
        auto it = m_Actions.find(m_MovePosition.State);
        if (it != m_Actions.end())
            it->second();
    }

    _UpdatePosition(p3DEngine, fDeltaTime);

    m_PrevMovePosition = m_MovePosition;
    return S_OK;
}

HRESULT LCharacter::Create(IL3DEngine* p3DEngine, ILScene* pScene, const char* szPath)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    hr = ILModel::Create(p3DEngine, szPath, &m_p3DModel);
    HRESULT_ERROR_EXIT(hr);

    m_p3DScene = pScene;
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

    m_p3DModel->AttachModel(piModel);

    return S_OK;
}

HRESULT LCharacter::LoadSocket(const char* szPath, const char* szSocketName)
{
    ILModel* piModel = nullptr;

    ILModel::Create(m_p3DEngine, szPath, &piModel);
    m_Parts.emplace_back(piModel);

    piModel->BindToSocket(m_p3DModel, szSocketName);

    return S_OK;
}

HRESULT LCharacter::SetPosition(const XMFLOAT3& Position)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    BOOL_ERROR_EXIT(m_p3DModel);

    hr = m_p3DModel->SetTranslation(Position);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT LCharacter::PlayAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority)
{
    return m_p3DModel->PlayAnimation(szAnimation, nPlayType, nPriority);
}

void LCharacter::AppendRenderEntity(ILScene* piScene)
{ 
    piScene->AddRenderEntity(m_p3DModel);
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
    if (m_MovePosition.State == LState::Idle)
        m_MovePosition.State = LState::Forward;

    m_MoveCtrl.bMove = true;
    m_MoveCtrl.nDirection8 = 0; // Forward
}

void LCharacter::KeyUpEvent()
{
    if (m_MovePosition.State == LState::Forward)
        m_MovePosition.State = LState::Idle;

    m_MoveCtrl.bMove = false;
}

void LCharacter::_UpdatePosition(IL3DEngine* p3DEngine, float fDeltaTime)
{
    XMFLOAT3 vPos;

    m_p3DScene->LogicalToScene(vPos, m_MovePosition.nX, m_MovePosition.nY, m_MovePosition.nZ);
    m_p3DModel->SetTranslation(vPos);
}
