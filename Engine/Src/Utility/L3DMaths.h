#pragma once

#include "L3DInterface.h"

struct RTS;

namespace L3D {

    inline void XMFloat3Slerp(XMFLOAT3* pResult, const XMFLOAT3* p0, const XMFLOAT3* p1, float fWeight) {
        XMStoreFloat3(pResult, XMVectorLerp(XMLoadFloat3(p0), XMLoadFloat3(p1), fWeight));
    }

    inline void XMFloat4Slerp(XMFLOAT4* pResult, const XMFLOAT4* p0, const XMFLOAT4* p1, float fWeight) {
        XMStoreFloat4(pResult, XMVectorLerp(XMLoadFloat4(p0), XMLoadFloat4(p1), fWeight));
    }

    void RTS2Matrix(XMMATRIX& mResult, const RTS& rts);
}