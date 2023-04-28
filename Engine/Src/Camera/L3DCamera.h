#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "L3DInterface.h"
#include "Define/L3DCameraDefine.h"

using namespace DirectX;

class L3DCamera : public ILCamera
{
public:
    HRESULT Init();
    HRESULT UpdateYawPitchRoll(float fYaw, float fPitch, float fRoll);
    HRESULT UpdateSightDistance(float fSightDis);

    virtual void SetTarget(const XMVECTOR vTarget);
    virtual void SetPosition(const XMVECTOR vPosition);

    void GetProperty(CAMERA_PROPERTY& Property);
    void SetProperty(const CAMERA_PROPERTY& Property);

    const CAMERA_INFO& GetCameraInfo() { return m_CameraInfo; };

private:
    float m_fYaw   = 1.5f * XM_PI;
    float m_fPitch = 0.25f * XM_PI;
    float m_fRoll  = 0.f;

    float m_fSightDis = 100.f;

    XMVECTOR m_vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR m_vTarget = XMVectorZero();
    XMVECTOR m_vPostion = XMVectorZero();

    CAMERA_INFO m_CameraInfo;

    void _UpdateViewMatrix();
    void _UpdateProjectMatrix();
};