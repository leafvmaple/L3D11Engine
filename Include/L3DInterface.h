#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include "LExports.h"
using namespace DirectX;

#define L3DX_2PI                XM_PI
#define LENGIEN_FILENAME_MAX    260
#define LENGIEN_FONT_STRING_MAX 260

#define GRAPHICS_LEVEL_COUNT 2
#define GRAPHICS_LEVEL_MIN 0
#define GRAPHICS_LEVEL_MAX 1

#define VERTEX_FVF       D3DFVF_XYZ
#define LIGHT_VERTEX_FVF VERTEX_FVF       | D3DFVF_NORMAL
#define COLOR_VERTEX_FVF LIGHT_VERTEX_FVF | D3DFVF_DIFFUSE
#define TEX_VERTEX_FVF   COLOR_VERTEX_FVF | D3DFVF_TEX1

static const BOOL GRAPHICS_LEVEL = 1;

enum LOBJECT_MESH_TYPE
{
    LOBJECT_MESH_TEAPOT,
    LOBJECT_MESH_DX,
    LOBJECT_MESH_LX,
    LOBJECT_MESH_COUNT,
};

struct Vertex
{
    Vertex(){}
    Vertex(float x, float y, float z)
    {
        _x = x;
        _y = y;
        _z = z;
    }
    float _x, _y, _z;
};

struct LightVertex : Vertex
{
    LightVertex() {}
    LightVertex(float x, float y, float z) : Vertex(x, y, z) {}
    void SetNormal(XMFLOAT3 vNormal)
    {
        _nx = vNormal.x;
        _ny = vNormal.y;
        _nz = vNormal.z;
    }
    float _nx, _ny, _nz;
    XMFLOAT3 operator - (CONST LightVertex& v) const {return XMFLOAT3(_nx - v._nx, _nx - v._nx, _nz - v._nz);}
};

struct ColorVertex : LightVertex
{
    ColorVertex(){}
    ColorVertex(float x, float y, float z, XMVECTORF32 color) : LightVertex(x, y, z)
    {
        _color = color;
    }
    XMVECTORF32 _color;
};

struct TexVertex : ColorVertex
{
    TexVertex(){}
    TexVertex(float x, float y, float z, XMVECTORF32 color, float u, float v) : ColorVertex(x, y, z, color)
    {
        _u = u;
        _v = v;
    }
    float _u, _v;
};

struct ADAPTERMODE
{
    UINT uWidth;
    UINT uHeight;
    UINT uRefreshRates[32];
};

struct L3DWINDOWPARAM
{
    int x;
    int y;
    int Width;
    int Height;
    BOOL bWindow;
    LPCWSTR lpszClassName;
    LPCWSTR lpszWindowName;
};

enum LOBJECT_TYPE
{
    LOBJECT_TYPE_INVALID,
    LOBJECT_TYPE_VERTEX,
    LOBJECT_TYPE_MESH,
    LOBJECT_TYPE_COUNT,
};

class L3DENGINE_CLASS IL3DEngine
{
public:
    virtual ~IL3DEngine() {};

    virtual HRESULT Init(HINSTANCE hInstance, L3DWINDOWPARAM& WindowParam) = 0;
    virtual HRESULT Uninit() = 0;

    virtual HRESULT Update(float fDeltaTime) = 0;

    virtual BOOL IsActive() = 0;

    static IL3DEngine* Instance();

protected:
    IL3DEngine() {};

private:
    static IL3DEngine* m_pInstance;
};
