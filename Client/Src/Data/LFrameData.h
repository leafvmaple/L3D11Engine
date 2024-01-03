#pragma once

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

struct Position
{
    int nX = 0;
    int nY = 0;
    int nZ = 0;

    float fUpdateTime = 0.f;
};

const int FRAME_INTERVAL = 1000 / 16;

class LFrameData
{
public:
    Position m_Position;
    LState m_State = LState::Idle;

    void AddPosition(int nX, int nY, int nZ);

    void SetState(LState state);
};