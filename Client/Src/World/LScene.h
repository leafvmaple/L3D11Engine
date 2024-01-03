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

    void Create(const char* szFileName);
    void Update(float fDeltaTime);
    void FrameMove(unsigned int nFrame);

    LCharacter* AddCharacter(const char* szFileName, bool bAvatar);

    LCharacter* m_pAvatar = nullptr;
    LCamera* m_pCamera = nullptr;
private:
    std::list<LModel*> m_Remotes;
    ILScene* m_piScene = nullptr;
};