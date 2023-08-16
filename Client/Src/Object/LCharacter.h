#pragma once
#include "LModel.h"
#include <vector>

class LCharacter : public LModel
{
public:
    LCharacter() {};
    virtual ~LCharacter() {};
    virtual HRESULT Display(IL3DEngine* p3DEngine, float fDeltaTime) { return S_OK; };

    HRESULT Create(IL3DEngine* p3DEngine, const char* szPath);
    HRESULT LoadPart(const char* szPath);
    HRESULT LoadSocket(const char* szPath, const char* szSocketName);

    HRESULT SetPosition(const XMFLOAT3& Position);

    HRESULT PlayAnimation(const char* szAnimation, ANIMATION_PLAY_TYPE nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority);

    void AppendRenderEntity(ILScene* piScene);

private:
    ILModel* m_pObject = nullptr;
    IL3DEngine* m_p3DEngine = nullptr;

    std::vector<ILModel*> m_Parts;
};

