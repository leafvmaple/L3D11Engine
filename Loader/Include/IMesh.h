#pragma once

#include "LInterface.h"
#include "LAssert.h"
#include "L3DFormat.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

typedef char NAME_STRING[30];

enum D3DFVF
{
    FVF_XYZW    = 0x001,
    FVF_NORMAL  = 0x002,
    FVF_COLOR   = 0x004,
    FVF_TEX1    = 0x008,
    FVF_SKIN    = 0x010,
    FVF_TANGENT = 0x020,
};

struct MESH_DESC
{
    const char* szName = nullptr;
};

struct BONE_SOURCE
{
    NAME_STRING szName = { 0 };

    int nChildCount = 0;
    NAME_STRING* pChildNames = nullptr;

    XMFLOAT4X4 mOffset;
    XMFLOAT4X4 mOffset2Parent;
    XMFLOAT4X4 mInvPxPose;
    L3D_BOUND_BOX BoundingBox;

    ~BONE_SOURCE()
    {
        SAFE_DELETE_ARRAY(pChildNames);
    }
};

struct SOCKET_SOURCE
{
    NAME_STRING szName = { 0 };
    NAME_STRING szParentName = { 0 };
    XMFLOAT4X4 mOffset;
};

static const unsigned VERTEX_MAX_BONE = 4;
struct VERTEX_SOURCE
{
    XMFLOAT3 Position = { 0.f, 0.f, 0.f };
    XMFLOAT3 Normal = { 0.f, 0.f, 0.f };
    XMCOLOR Color = { 0, 0, 0, 0 };
    XMFLOAT2 TexCoords = { 0.f, 0.f };
    float BoneWeights[VERTEX_MAX_BONE] = { 0.f };
    BYTE BoneIndices[VERTEX_MAX_BONE] = { 0xFF, 0xFF, 0xFF, 0xFF };
    XMFLOAT4 Tangent = { 0.0f, 1.0f, 0.0f, 1.0f };
};

struct MESH_SOURCE : LUnknown
{
    int nVertexFVF = 0;
    int nVertexSize = 0;
    int nFacesCount = 0;

    int nVerticesCount = 0;
    VERTEX_SOURCE* pVertices = nullptr;

    int nIndexCount = 0;
    DWORD* pIndices = nullptr;

    int nBonesCount = 0;
    BONE_SOURCE* pBones = nullptr;

    int nSocketCount = 0;
    SOCKET_SOURCE* pSockets = nullptr;

    int nSubsetCount = 0;
    WORD* pSubsetVertexCount = nullptr;

    ~MESH_SOURCE()
    {
        SAFE_DELETE_ARRAY(pVertices);
        SAFE_DELETE_ARRAY(pIndices);
        SAFE_DELETE_ARRAY(pBones);
        SAFE_DELETE_ARRAY(pSockets);
        SAFE_DELETE_ARRAY(pSubsetVertexCount);
    }
};

L3DENGINE_API void LoadMesh(MESH_DESC* pDesc, MESH_SOURCE*& pSource);