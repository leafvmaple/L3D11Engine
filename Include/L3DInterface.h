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

#define GAME_FPS 16

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
    Vertex() {}
    Vertex(float x, float y, float z) : _x(x), _y(y), _z(z) {}
    float _x = .0f;
    float _y = .0f;
    float _z = .0f;
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
    float _nx = 0.f;
    float _ny = 0.f;
    float _nz = 0.f;
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
    float _u = 0.f;
    float _v = 0.f;
};

struct ADAPTERMODE
{
    UINT uWidth;
    UINT uHeight;
    UINT uRefreshRates[32];
};

struct L3D_WINDOW_PARAM
{
    int x;
    int y;
    int Width;
    int Height;
    BOOL bWindow;
    LPCWSTR lpszClassName;
    LPCWSTR lpszWindowName;

    HWND wnd;
};

enum LOBJECT_TYPE
{
    LOBJECT_TYPE_INVALID,
    LOBJECT_TYPE_VERTEX,
    LOBJECT_TYPE_MESH,
    LOBJECT_TYPE_COUNT,
};

enum SPLIT_TYPE
{
    SPLIT_TOP,    // 上半身
    SPLIT_BOTTOM, // 下半身
    SPLIT_ALL,
    SPLIT_COUNT,
};

enum ANIMATION_CONTROLLER_PRIORITY
{
    ANICTL_PRIMARY,   // 低轨
    ANICTL_SECONDARY, // 高轨
    ANICTL_COUNT,
    ANICTL_FORCEDWORD = 0xffffffff,
};

enum class AnimationPlayType
{
    Circle,  // 循环播放动作
    Once,    // 单次播放动作
    ForceDword = -1,
};

class L3DENGINE_CLASS IL3DEngine
{
public:
    virtual ~IL3DEngine() {};

    virtual HRESULT Init(HINSTANCE hInstance, L3D_WINDOW_PARAM& WindowParam) = 0;
    virtual HRESULT Uninit() = 0;

    virtual HRESULT Update(float fDeltaTime) = 0;

    virtual ID3D11Device* GetDevice() const = 0;
    virtual DWORD GetFrame() const = 0;
    virtual float GetTime() const = 0;

    static IL3DEngine* Instance();
    static void Destroy();

protected:
    IL3DEngine() {};

private:
    static IL3DEngine* m_pInstance;
};

class L3DENGINE_CLASS ILModel
{
public:
    static HRESULT Create(IL3DEngine* pL3DEngine, const char* szFileName, ILModel** ppModel);

    virtual void AttachModel(ILModel* pModel) = 0;
    virtual void BindToSocket(ILModel* pModel, const char* szSocketName) = 0;

    virtual HRESULT ResetTransform() = 0;
    virtual HRESULT SetTranslation(const XMFLOAT3& Translation) = 0;
    virtual HRESULT SetRotation(const XMFLOAT4& Rotation) = 0;
    virtual HRESULT SetScale(const XMFLOAT3& Scale) = 0;

    virtual HRESULT PlayAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority) = 0;
    virtual HRESULT PlaySplitAnimation(const char* szAnimation, SPLIT_TYPE nSplitType, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority) = 0;
};

class L3DENGINE_CLASS ILCamera
{
public:
    virtual void SetTarget(const XMVECTOR vTarget) = 0;
    virtual void SetPosition(const XMVECTOR vPosition) = 0;
};

class L3DENGINE_CLASS ILScene
{
public:
    static HRESULT Create(IL3DEngine* pL3DEngine, const char* szFileName, ILScene** ppScene);

    virtual ILCamera* Get3DCamera() = 0;
    virtual void AddRenderEntity(ILModel* pModel) = 0;

    virtual void GetFloor(const XMFLOAT3& vPos, float& fHeight) = 0;
    virtual void LogicalToScene(XMFLOAT3& vPos, int nX, int nY, int nZ) = 0;
};