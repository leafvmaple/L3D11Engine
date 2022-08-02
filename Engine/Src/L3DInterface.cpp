#include "LAssert.h"
#include "L3DInterface.h"
#include "L3DEngine.h"

IL3DEngine* IL3DEngine::m_pInstance = NULL;

IL3DEngine* IL3DEngine::Instance()
{
    if (!m_pInstance)
        m_pInstance = new L3DEngine;
    return m_pInstance;
}
