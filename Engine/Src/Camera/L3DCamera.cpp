#include "L3DCamera.h"

HRESULT L3DCamera::Init(float fWidth, float fHeight)
{
    m_Projection = XMMatrixPerspectiveFovLH(0.25f * XM_PI, fWidth / fHeight, 1.0f, 1000.0f);

    ComputeViewMatrix();

    return S_OK;
}

HRESULT L3DCamera::UpdateYawPitchRoll(float fYaw, float fPitch, float fRoll)
{
    m_fYaw += fYaw;
    m_fPitch += fPitch;
    m_fRoll += fRoll;

    return ComputeViewMatrix();
}

HRESULT L3DCamera::UpdateSightDistance(float fSightDis)
{
    m_fSightDis += fSightDis;
    m_fSightDis = max(m_fSightDis, 3.f);

    ComputeViewMatrix();

    return S_OK;
}

XMMATRIX L3DCamera::GetWorldViewProjcetion(const XMMATRIX& world)
{
    return world * m_View * m_Projection;
}

HRESULT L3DCamera::ComputeViewMatrix()
{
    float x = m_fSightDis * sinf(m_fPitch) * cosf(m_fYaw);
    float z = m_fSightDis * sinf(m_fPitch) * sinf(m_fYaw);
    float y = m_fSightDis * cosf(m_fPitch);

    XMVECTOR m_vPostion = XMVectorSet(x, y, z, 1.0f);

    m_View = XMMatrixLookAtLH(m_vPostion, m_vTarget, m_vUp);

    return S_OK;
}
