#pragma once
#include "LModel.h"

class LCharacter : public LModel
{
public:
    LCharacter() {};
    virtual ~LCharacter() {};
    virtual HRESULT Display(IL3DEngine* p3DEngine, float fDeltaTime) { return S_OK; };

    HRESULT Create(IL3DEngine* p3DEngine, const char* szPath);

private:
    ILModel* m_pObject = nullptr;
};

