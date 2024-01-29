#pragma once

#include <list>

class ILScene;
class IL3DEngine;

class LModel;
class LCharacter;
class LCamera;

class LScene
{
public:
    ~LScene();

    bool Create(const char* szFileName);
    void FrameMove(unsigned int nFrame);
    void Update(float fDeltaTime);

    LCharacter* AddCharacter(const char* szFileName, bool bAvatar);

    LCharacter* m_pAvatar = nullptr;
    LCamera* m_pCamera = nullptr;
private:
    std::list<LModel*> m_Remotes;

    ILScene* m_p3DScene = nullptr;
    IL3DEngine* m_p3DEngine = nullptr;
};