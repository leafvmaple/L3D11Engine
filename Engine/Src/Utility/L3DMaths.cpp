#include "stdafx.h"

#include "L3DMaths.h"
#include "IAnimation.h"

namespace L3D {
    // RTS×ª¾ØÕó
    void RTS2Matrix(XMMATRIX& mResult, const RTS& rts) {
        XMMATRIX mRot = XMMatrixTranspose(XMMatrixRotationQuaternion(XMLoadFloat4(&rts.Rotation)));
        XMMATRIX mScale = XMMatrixScaling(rts.Scale.x, rts.Scale.y, rts.Scale.z);

        // construct matScaleSign * matTrans
        XMMATRIX mScaleSignAndTrans = {
            XMVectorSet(rts.Sign, 0, 0, 0),
            XMVectorSet(0, rts.Sign, 0, 0),
            XMVectorSet(0, 0, rts.Sign, 0),
            XMVectorSet(rts.Translation.x, rts.Translation.y, rts.Translation.z, 1.0f)
        };

        mResult = XMMatrixMultiply(mScale, mRot);
        mResult = XMMatrixMultiply(mResult, mScaleSignAndTrans);
    }
}