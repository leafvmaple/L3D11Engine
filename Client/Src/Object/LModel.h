#pragma once

#include "L3DInterface.h"

class L3DENGINE_CLASS ILModel;

class LModel
{
public:
    LModel() {};
    virtual ~LModel() {};

    virtual HRESULT FrameMove(IL3DEngine* p3DEngine, unsigned int nFrame) = 0;
    virtual HRESULT Update(IL3DEngine* p3DEngine, float fDeltaTime) = 0;

protected:
    ILModel* m_pObject = nullptr;
};