#pragma once

#include "L3DInterface.h"

class ILCamera;

class LCamera
{
public:
    void Init(ILCamera* piCamera);
    void AddSightDistance(float fSightDis);
    void SetDistance(float fSightDis);
    void SetYaw(float fYaw);
    void SetPitch(float fPitch);

    void AddYaw(float fYaw);
    void AddPitch(float fPitch);

    void SetTarget(const XMFLOAT3& vTarget);
    void GetTarget(XMFLOAT3& vTarget);
    void MoveTarget(const XMVECTOR& vTarget);

    void Update();

private:
    ILCamera* m_piCamera = nullptr;

    float m_fYaw = 1.5f * XM_PI;
    float m_fPitch = 0.25f * XM_PI;
    float m_fRoll = 0.f;

    float m_fSightDis = 100.f;

    XMVECTOR m_vTarget = XMVectorZero();
    XMVECTOR m_vPostion = XMVectorZero();

    void _Format(float& fValue);
};