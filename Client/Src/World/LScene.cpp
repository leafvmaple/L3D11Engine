#include "LScene.h"

#include "L3DInterface.h"
#include "LCamera.h"
#include "Object/LModel.h"
#include "Object/LCharacter.h"

#include "LAssert.h"

LScene::~LScene()
{
    for (auto& object : m_Remotes)
        SAFE_DELETE(object);
}

void LScene::Create(const char* szFileName)
{
    LCharacter* pCharacter = nullptr;

    ILScene::Create(IL3DEngine::Instance(), szFileName, &m_piScene);
    
    m_pCamera = new LCamera;
    m_pCamera->Init(m_piScene->Get3DCamera());
    m_pCamera->SetYaw(-0.25 * XM_PI);
    m_pCamera->SetPitch(-0.1 * XM_PI);
    m_pCamera->SetDistance(200);

    pCharacter = AddCharacter("data/source/player/F1/²¿¼þ/Mdl/F1.mdl", true);

    pCharacter->AppendRenderEntity(m_piScene);
    pCharacter->SetPosition(XMFLOAT3(0, 0, 0));
}

void LScene::Update(float fDeltaTime)
{
    m_pAvatar->Display(IL3DEngine::Instance(), fDeltaTime);
    for (auto& remote : m_Remotes)
        remote->Display(IL3DEngine::Instance(), fDeltaTime);
    m_pCamera->Update();
}

LCharacter* LScene::AddCharacter(const char* szFileName, bool bAvatar)
{
    LCharacter* pObject = new LCharacter;
    if (bAvatar)
        m_pAvatar = pObject;
    else
        m_Remotes.push_back(pObject);
    pObject->Create(IL3DEngine::Instance(), szFileName);
    return pObject;
}
