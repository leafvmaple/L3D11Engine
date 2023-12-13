#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct MESH_SHARED_CB
{
    DirectX::XMFLOAT4X4 MatrixWorld;
};

enum RUNTIME_MACRO
{
    RUNTIME_MACRO_MESH,
    RUNTIME_MACRO_SKIN_MESH,
    RUNTIME_MACRO_TERRAIN,
};

__declspec(align(16)) struct SHARED_SHADER_COMMON
{
    __declspec(align(16)) struct SWITCH
    {
        int bEnableSunLight = 0;
        int bEnableConvertMap = 0;
        int bEnableIBL = 0;
        int bEnableFog = 0;
    };

    __declspec(align(16)) struct CAMERA
    {
        DirectX::XMMATRIX CameraView;
        DirectX::XMMATRIX CameraProject;
    };

    SWITCH Switch;
    CAMERA Camera;
};

struct MTLSYS_TERRAIN_CB
{
    DirectX::XMFLOAT2 UnitScaleXZ;
    DirectX::XMINT2   RegionOffset;
    DirectX::XMFLOAT2 WorldRegionOffset;
    float    HeightMapUVScale;
    float    HeightMapUVBias;
};

struct MTLSYS_TERRAIN_TEXTURES
{
    ID3D11ShaderResourceView* g_TerrainHeightMap = nullptr;
    ID3D11ShaderResourceView* g_TerrainNormalMap = nullptr;
};

struct SHARED_SHADER_SAMPLERS
{
    ID3D11SamplerState* pSamplerPointClamp = nullptr;        // register(s15);
};