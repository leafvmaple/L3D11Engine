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

float3x3 invert_3x3( float3x3 M )
{
    float D = determinant( M );		
    float3x3 T = transpose( M );	

    return float3x3(
        cross( T[1], T[2] ),
        cross( T[2], T[0] ),
        cross( T[0], T[1] ) ) / D;	
}

float3x3 GetMatrixForNormal(float4x4 matWorld)
{
    float3x3 WorldForNormal;
    WorldForNormal = invert_3x3((float3x3)matWorld);
    WorldForNormal = transpose(WorldForNormal);

    return WorldForNormal;
}

#endif