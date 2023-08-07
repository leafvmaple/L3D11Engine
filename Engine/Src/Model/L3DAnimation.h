#pragma once

#include "L3DInterface.h"
#include <vector>
#include <string>

class L3DAnimation
{
public:
    HRESULT LoadFromFile(const char* szAnimation);

private:
    DWORD m_dwNumBone    = 0;
    DWORD m_dwNumFrames  = 0;
    float m_fFrameLength = 0.f;
    DWORD m_dwAniLen     = 0;

    std::string m_szName;
    std::vector<std::string> m_BoneNames;
    std::vector<RTS> m_BoneRTS;
};