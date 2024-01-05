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


struct CHARACTER_MOVE_POSITION
{
    int nX = 0;
    int nY = 0;
    int nZ = 0;

    LState State = LState::Idle;
};

struct CHARACTER_FRAME_DATA
{
    int nGameLoop = 0;
    CHARACTER_MOVE_POSITION PositionData;
};

const int FRAME_INTERVAL = 1000 / 16;

class LFrameData
{
public:
    void FillFrameData(int nGameLoop, CHARACTER_MOVE_POSITION* pUpdateData);
};