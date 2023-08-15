#include "LScene.h"

#include "L3DInterface.h"
#include "LCamera.h"
#include "LObjectMgr.h"
#include "Object/LCharacter.h"

void LScene::Create(const char* szFileName)
{
    LCharacter* pCharacter = nullptr;

    ILScene::Create(IL3DEngine::Instance(), "maps/唐门登陆界面/唐门登陆界面.jsonmap", &m_piScene);
    
    m_pCamera = new LCamera;
    m_pCamera->Init(m_piScene->Get3DCamera());
    m_pCamera->SetYaw(-0.25 * XM_PI);
    m_pCamera->SetPitch(-0.1 * XM_PI);

    pCharacter = LObjectMgr::Instance().CreateModel<LCharacter>("data/source/player/F1/部件/Mdl/F1.mdl");
    pCharacter->LoadPart(IL3DEngine::Instance(), "data/source/player/F1/部件/F1_0000_head.mesh");
    pCharacter->LoadPart(IL3DEngine::Instance(), "data/source/player/F1/部件/F1_1000_body.mesh");
    pCharacter->LoadPart(IL3DEngine::Instance(), "data/source/player/F1/部件/f1_new_face.Mesh", "s_face");
    pCharacter->AppendRenderEntity(m_piScene);
    pCharacter->SetPosition(XMFLOAT3(0, 0, 0));
    // pCharacter->PlayAnimation("Res/Animation/A055_st01.ani", ENU_ANIMATIONPLAY_CIRCLE, ANICTL_PRIMARY);
}

void LScene::Update(float fDeltaTime)
{
    m_pCamera->Update();
}
