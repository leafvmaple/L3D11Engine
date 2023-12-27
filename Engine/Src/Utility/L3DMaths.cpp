#include "stdafx.h"

#include "L3DMaths.h"
#include "IAnimation.h"

namespace L3D {
    // RTS×ª¾ØÕó
    void RTS2Matrix(XMMATRIX& mResult, const RTS& rts, int nFlag) {
        if (nFlag & BONE_FLAG_AFFLINE)
            mResult = XMMatrixAffineTransformation(
                XMLoadFloat3(&rts.Scale), g_XMZero, XMLoadFloat4(&rts.Rotation), XMLoadFloat3(&rts.Translation));
        else
            mResult = XMMatrixTransformation(g_XMZero, XMLoadFloat4(&rts.SRotation),
            XMLoadFloat3(&rts.Scale), g_XMZero, XMLoadFloat4(&rts.Rotation), XMLoadFloat3(&rts.Translation));
    }
}