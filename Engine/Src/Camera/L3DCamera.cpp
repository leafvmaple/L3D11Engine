#include "stdafx.h"

#include <algorithm>

#include "L3DCamera.h"


HRESULT L3DCamera::Init()
{
    return S_OK;
}

HRESULT L3DCamera::UpdateYawPitchRoll(float fYaw, float fPitch, float fRoll)
{
    m_fYaw += fYaw;
    m_fPitch += fPitch;
    m_fRoll += fRoll;

    _UpdateViewMatrix();
    return S_OK;
}

HRESULT L3DCamera::UpdateSightDistance(float fSightDis)
{
    m_fSightDis += fSightDis;
    m_fSightDis = std::max(m_fSightDis, 3.f);

    _UpdateViewMatrix();

    return S_OK;
}


void L3DCamera::SetTarget(const XMVECTOR vTarget)
{
    m_vTarget = vTarget;
    _UpdateViewMatrix();
}


void L3DCamera::SetPosition(const XMVECTOR vPosition)
{
    m_vPostion = vPosition;
    _UpdateViewMatrix();
}

void L3DCamera::GetProperty(CAMERA_PROPERTY& Property)
{
    memcpy(&Property, &m_CameraInfo.Property, sizeof(Property));
}


void L3DCamera::SetProperty(const CAMERA_PROPERTY& Property)
{
    memcpy(&m_CameraInfo.Property, &Property, sizeof(Property));

    _UpdateProjectMatrix();
}

void L3DCamera::_UpdateViewMatrix()
{
    if (!XMVector2Equal(m_vPostion, m_vTarget))
        m_CameraInfo.mView = XMMatrixLookAtLH(m_vPostion, m_vTarget, m_vUp);
}

void L3DCamera::_UpdateProjectMatrix()
{
    m_CameraInfo.mProject = XMMatrixPerspectiveFovLH(m_CameraInfo.Property.Persective.fFovAngleY, m_CameraInfo.Property.Persective.fAspectRatio, 1.0f, 1000.0f);
}
