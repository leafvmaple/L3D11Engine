#include "LScene.h"

#include "L3DInterface.h"
#include "LCamera.h"
#include "LObjectMgr.h"
#include "Object/LCharacter.h"

void LScene::Create(const char* szFileName)
{
    LCharacter* pCharacter = nullptr;

    ILScene::Create(IL3DEngine::Instance(), "Res/maps/唐门登陆界面/唐门登陆界面.jsonmap", &m_piScene);
    
    m_pCamera = new LCamera;
    m_pCamera->Init(m_piScene->Get3DCamera());

    pCharacter = LObjectMgr::Instance().CreateModel<LCharacter>("Res/Mesh/A055_body.mesh");
    pCharacter->AppendRenderEntity(m_piScene);
    pCharacter->SetPosition(XMFLOAT3(0, -45, 0));
}

void LScene::Update(float fDeltaTime)
{
    m_pCamera->Update();
}
