#include "stdafx.h"

#include "L3DMaths.h"
#include "IAnimation.h"

namespace L3D {
    // RTS×ª¾ØÕó
    void RTS2Matrix(XMMATRIX& mResult, const RTS& rts, int nAffline) {
        if (nAffline)
            mResult = XMMatrixAffineTransformation(
                XMLoadFloat3(&rts.Scale), g_XMZero, XMLoadFloat4(&rts.Rotation), XMLoadFloat3(&rts.Translation));
        else
            mResult = XMMatrixTransformation(g_XMZero, XMQuaternionIdentity(),
            XMLoadFloat3(&rts.Scale), g_XMZero, XMLoadFloat4(&rts.Rotation), XMLoadFloat3(&rts.Translation));
    }
}