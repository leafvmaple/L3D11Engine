#ifndef __CommonParam_H 
#define __CommonParam_H

SamplerState g_sPointClamp : register(s15);

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

float4 GetDeviceCoordinate(float4 InputPosition, float4x4 WorldMatrix, float4x4 ViewMatrix, float4x4 ProjMatrix)
{
    return mul(mul(mul(InputPosition, WorldMatrix), ViewMatrix), ProjMatrix); //12 ALU
}

#endif