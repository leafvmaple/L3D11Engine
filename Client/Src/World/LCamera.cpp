#include "LCamera.h"
#include <algorithm>

void LCamera::Init(ILCamera* piCamera)
{
    m_piCamera = piCamera;
}

void LCamera::SetSightDistance(float fSightDis)
{
    m_fSightDis += fSightDis;
    m_fSightDis = std::max(m_fSightDis, 3.f);
}

void LCamera::Update()
{
    float x = m_fSightDis * sinf(m_fPitch) * cosf(m_fYaw);
    float z = m_fSightDis * sinf(m_fPitch) * sinf(m_fYaw);
    float y = m_fSightDis * cosf(m_fPitch);

    m_vPostion = m_vTarget + XMVectorSet(x, y, z, 1.0f);

    m_piCamera->SetTarget(m_vTarget);
    m_piCamera->SetPosition(m_vPostion);
}
