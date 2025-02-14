#pragma once

#include <d3d11.h>

#include "L3DFormat.h"

#define _ID_STR(_ID) _ID, #_ID
#define IDR_SHADER_SIMPLE 117

struct L3D_SHADER_TABLE
{
    ID3D11VertexShader* Vertex[L3D_VERTEX_SHADER_COUNT] = {0};
    ID3D11PixelShader*  Pixel[L3D_PIXEL_SHADER_COUNT]   = {0};
    ID3D11InputLayout*  Layout[L3D_INPUT_LAYOUT_COUNT]  = {0};
};

struct _SHADER_INFO
{
    union
    {
        int nID;
        L3D_VERTEX_SHADER eVertexShaderID;
        L3D_PIXEL_SHADER  ePixelShaderID;
    } ID;
    unsigned    uShaderID = 0;
    const char* pcszSourceName  = nullptr;
    const char* pcszEntryPoint  = nullptr;
    const char* pcszShaderModel = nullptr;
    const char* pcszMacroList   = nullptr;
};

static const struct _LAYOUT_INFO
{
    L3D_INPUT_LAYOUT  eInputLayout;
    L3D_VERTEX_SHADER eVertexShader;
    unsigned int uDescCount;
    D3D11_INPUT_ELEMENT_DESC DescArr[30];
} INPUT_LAYOUT_LIST[] =
{
    {
        L3D_INPUT_LAYOUT_CI_MESH,
        L3D_VERTEX_SHADER_CI_MESH,
        5,
        {
            {"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,     0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR",       0, DXGI_FORMAT_B8G8R8A8_UNORM,      0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,        0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT",     0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0}
        }
    },
    {
        L3D_INPUT_LAYOUT_CI_SKINMESH,
        L3D_VERTEX_SHADER_CI_SKINMESH,
        7,
        {
            {"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,     0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR",       0, DXGI_FORMAT_B8G8R8A8_UNORM,      0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,        0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT,       0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT",     0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0}
        }
    },
    {
        L3D_INPUT_LAYOUT_CI_TERRAIN,
        L3D_VERTEX_SHADER_CI_TERRAIN,
        3,
        {
            {"POSITION",    0, DXGI_FORMAT_R32G32_FLOAT,        0,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA,    0},
            {"CINSTANCE",   0, DXGI_FORMAT_R32G32B32A32_SINT,   1,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_INSTANCE_DATA,  1},
            {"CINSTANCE",   1, DXGI_FORMAT_R32G32B32A32_FLOAT,  1,  D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_INSTANCE_DATA,  1},
        }
    },
};

/*
* Create Vertex Shader
* Create Pixel Shader
* Create Input Layout
*/
bool CreateShaderTable(std::shared_ptr<L3D_SHADER_TABLE>* pShader, ID3D11Device* piDevice);