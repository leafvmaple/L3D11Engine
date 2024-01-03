#pragma once
#include "LModel.h"
#include <vector>

#include "boost/sml.hpp"

#include "Data/LFrameData.h"

namespace sml = boost::sml;

enum class LEvent
{
    Forward,
    KeyUp,
};

class LCharacter : public LModel
{
public:
    LCharacter() {};
    virtual ~LCharacter() {};
    virtual HRESULT FrameMove(IL3DEngine* p3DEngine, unsigned int nFrame);
    virtual HRESULT Update(IL3DEngine* p3DEngine, float fDeltaTime);

    HRESULT Create(IL3DEngine* p3DEngine, const char* szPath);
    HRESULT LoadPart(const char* szPath);
    HRESULT LoadSocket(const char* szPath, const char* szSocketName);

    HRESULT SetPosition(const XMFLOAT3& Position);

    HRESULT PlayAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority);

    void AppendRenderEntity(ILScene* piScene);

    // Action
    void ForwardAction();
    void IdleAction();

    // Event
    void ProcessEvent(LEvent event);

    void ForwardEvent();
    void KeyUpEvent();

private:
    ILModel* m_pObject = nullptr;
    IL3DEngine* m_p3DEngine = nullptr;

    std::vector<ILModel*> m_Parts;

    LFrameData m_FrameData;
    LFrameData m_PrevFrameData;

private:
    void _UpdatePosition(IL3DEngine* p3DEngine, float fDeltaTime);
};

