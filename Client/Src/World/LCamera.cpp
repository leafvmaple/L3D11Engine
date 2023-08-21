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

void LCamera::SetYaw(float fYaw)
{
    m_fYaw = fYaw;
    _Format(m_fYaw);
}

void LCamera::SetPitch(float fPitch)
{
    m_fPitch = fPitch;
    _Format(m_fPitch);
}

void LCamera::AddYaw(float fYaw)
{
    SetYaw(m_fYaw + fYaw);
}

void LCamera::AddPitch(float fPitch)
{
    SetPitch(m_fPitch + fPitch);
}

void LCamera::SetTarget(const XMFLOAT3& vTarget)
{
    m_vTarget = XMLoadFloat3(&vTarget);
}

void LCamera::GetTarget(XMFLOAT3& vTarget)
{
    XMStoreFloat3(&vTarget, m_vTarget);
}

void LCamera::MoveTarget(const XMVECTOR& vTarget)
{
    m_vTarget += vTarget;
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

void LCamera::_Format(float& fValue)
{
    while (fValue > XM_2PI)
        fValue -= XM_2PI;
    while (fValue < -XM_2PI)
        fValue += XM_2PI;
}
