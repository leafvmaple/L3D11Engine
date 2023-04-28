#pragma once

class ILScene;
class IL3DEngine;

class LCamera;

class LScene
{
public:
    void Create(const char* szFileName);
    void Update(float fDeltaTime);

    LCamera* m_pCamera = nullptr;

private:
    ILScene* m_piScene = nullptr;
};