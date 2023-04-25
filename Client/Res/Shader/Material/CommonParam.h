#ifndef __CommonParam_H 
#define __CommonParam_H

struct SWITCH 
{
    int bEnableSunLight;
    int bEnableConvertMap;
    int bEnableIBL;
    int bEnableFog;
};

struct CAMERA
{
    row_major matrix CameraView           : View;
    row_major matrix CameraProject        : Project;
};

struct COMMON_PARAM
{
    SWITCH Switch;
    CAMERA Camera;
};

cbuffer CBUFFER_COMMON : register(b13)
{
    COMMON_PARAM g_Com;
};

#endif