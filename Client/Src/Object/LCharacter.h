#pragma once
#include "LModel.h"
#include <vector>
#include <unordered_map>
#include <functional>

#include "boost/sml.hpp"

#include "Data/LFrameData.h"

namespace sml = boost::sml;

enum class LEvent
{
    Forward,
    KeyUp,
};

struct KMOVE_CTRL
{
    BOOL bMove = 0;
    int  nDirection8 = 0;
};

class LCharacter : public LModel
{
public:
    LCharacter() {};
    virtual ~LCharacter() {};
    virtual HRESULT FrameMove(IL3DEngine* p3DEngine, unsigned int nFrame);
    virtual HRESULT Update(IL3DEngine* p3DEngine, float fDeltaTime);

    HRESULT Create(IL3DEngine* p3DEngine, ILScene* pScene, const char* szPath);
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
    IL3DEngine* m_p3DEngine = nullptr;
    ILScene* m_p3DScene = nullptr;
    ILModel* m_p3DModel = nullptr;

    std::vector<ILModel*> m_Parts;
    std::unordered_map<LState, std::function<void()>> m_Actions = {
        { LState::Forward, std::bind(&LCharacter::ForwardAction, this) },
        { LState::Idle, std::bind(&LCharacter::IdleAction, this) },
    };

    LFrameData m_FrameData;
    CHARACTER_MOVE_POSITION m_MovePosition;
    CHARACTER_MOVE_POSITION m_PrevMovePosition;

    KMOVE_CTRL m_MoveCtrl;

private:
    void _UpdatePosition(IL3DEngine* p3DEngine, float fDeltaTime);
};

