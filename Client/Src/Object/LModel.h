#pragma once

#include "L3DInterface.h"

class L3DENGINE_CLASS ILModel;

class LModel
{
public:
	LModel() { m_pObject = NULL; };
	virtual ~LModel() {};

	virtual HRESULT Display(IL3DEngine* p3DEngine, float fDeltaTime) = 0;

protected:
	ILModel* m_pObject;
};