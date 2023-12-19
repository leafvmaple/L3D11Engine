#pragma once
#include "LModel.h"
#include <vector>

#include "boost/sml.hpp"

namespace sml = boost::sml;

namespace LEvent
{
    struct Forward {};
    struct KeyUp {};
}

namespace LAction {
    struct Animation {
        template<class TMsg>
        constexpr void Play(const TMsg& msg) {}
    };

    constexpr auto PlayAnimation = []() {
        // action.Play(event);
    };
}

struct StateMachine {
    auto operator()() const {
        using namespace sml;
        // 定义状态转换表
        return make_transition_table(
            *state<class Idle> + event<LEvent::Forward> / LAction::PlayAnimation = state<class Running>,
            state<class Running> +event<LEvent::KeyUp> / LAction::PlayAnimation = state<class Idle>,
            state<class Idle> + event<LEvent::KeyUp> = X
        );
    }
};

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

    HRESULT PlayAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority);

    void AppendRenderEntity(ILScene* piScene);

    // Action

    void Forward();
    void KeyUp();

private:

    ILModel* m_pObject = nullptr;
    IL3DEngine* m_p3DEngine = nullptr;

    std::vector<ILModel*> m_Parts;

    sml::sm<StateMachine> m_StateMachine;
};

