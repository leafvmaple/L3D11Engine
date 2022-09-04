#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class L3DCamera
{
public:
    HRESULT Init(float fWidth, float fHeight);
    HRESULT UpdateYawPitchRoll(float fYaw, float fPitch, float fRoll);
    HRESULT UpdateSightDistance(float fSightDis);

    XMMATRIX GetWorldViewProjcetion(const XMMATRIX& world);

private:
    XMMATRIX m_Projection;
    XMMATRIX m_View;

    float m_fYaw   = 1.5f * XM_PI;
    float m_fPitch = 0.25f * XM_PI;
    float m_fRoll  = 0.f;

    float m_fSightDis = 100.f;

    XMVECTOR m_vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR m_vTarget = XMVectorZero();
    XMVECTOR m_vPostion = XMVectorZero();

    HRESULT ComputeViewMatrix();
};