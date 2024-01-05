#pragma once

#include <vector>
#include "L3Dinterface.h"

enum class LState
{
    Idle,
    Forward,
    Backward,
    Left,
    Right,
    Jump,
    Attack,
    Skill,
    Dead,
};

struct CHARACTER_MOVE_POSITION
{
    LState State = LState::Idle;

    int nX = 0;
    int nY = 0;
    int nZ = 0;
};

struct CHARACTER_FRAME_DATA
{
    int nGameLoop = 0;
    CHARACTER_MOVE_POSITION PositionData;
};

struct CHARACTER_INTERPOLATE_DATA
{
    LState State = LState::Idle;

    XMFLOAT3 vPosition = {};
};

class LFrameData
{
public:
    void Init();
    void UpdateFramesData(int nGameLoop, CHARACTER_MOVE_POSITION* pUpdateData);
    void Interpolate(IL3DEngine* p3DEngine, ILScene* p3DScene, float fTime);

    void GetPosition(XMFLOAT3& vPosition);
private:
    std::vector<CHARACTER_FRAME_DATA> m_FrameArray;

private:
    // GetFrameIndex
    int GetFrameIndex(int nGameLoop);
    // GetMaxGameLoopFrameIndex
    int GetMaxGameLoopFrameIndex();

    CHARACTER_INTERPOLATE_DATA m_Interpolate;    // 当前插值数据
};