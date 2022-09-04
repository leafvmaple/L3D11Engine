#pragma once

#include <d3d11.h>
#include <list>
#include "LAssert.h"
#include "L3DInterface.h"

class LModel;
class LScene;
class IL3DEngine;

class LObjectMgr
{
public:
    LObjectMgr();
    ~LObjectMgr();

    HRESULT Init(HINSTANCE hInstance, L3D_WINDOW_PARAM& WindowParam);
    void Uninit();

    template<typename T>
    T* CreateModel(const char* szFileName)
    {
        T* pObject = new T;
        m_ObjectList.push_back(pObject);
        pObject->Create(IL3DEngine::Instance(), szFileName);
        return pObject;
    }

    template<typename T>
    T* CreateModel()
    {
        T* pObject = new T;
        m_ObjectList.push_back(pObject);
        pObject->Create(IL3DEngine::Instance(), nullptr);
        return pObject;
    }

    HRESULT Update(float fDeltaTime);

    BOOL IsActive();

private:
    std::list<LModel*> m_ObjectList;
    std::list<LScene*> m_SceneList;
};
