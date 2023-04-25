#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct CAMERA_PROPERTY
{
    union
    {
        struct PERSPECTIVE_PARAM
        {
            float fFovAngleY;
            float fAspectRatio;
        } Persective;
    };
};

struct __declspec(align(16)) CAMERA_INFO
{
    XMMATRIX mView;
    XMMATRIX mProject;

    CAMERA_PROPERTY Property;
};