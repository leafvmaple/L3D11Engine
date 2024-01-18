#pragma once

#include <d3d11.h>
#include <DirectXPackedVector.h>

#include <filesystem>

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Colors
{
    XMGLOBALCONST XMFLOAT4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

    XMGLOBALCONST XMFLOAT4 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
    XMGLOBALCONST XMFLOAT4 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}

typedef struct L3D_BOUND_BOX
{
    float fMinX, fMinY, fMinZ;
    float fMaxX, fMaxY, fMaxZ;
} L3D_BOUND_BOX;

enum L3D_INPUT_LAYOUT
{
    L3D_INPUT_LAYOUT_CI_MESH,
    L3D_INPUT_LAYOUT_CI_SKINMESH,
    L3D_INPUT_LAYOUT_CI_TERRAIN,

    L3D_INPUT_LAYOUT_COUNT,
};

enum L3D_PIXEL_SHADER
{
    L3D_PIXEL_SHADER_NULL,
    L3D_PIXEL_SHADER_UV1_NO_LIGHT,

    L3D_PIXEL_SHADER_COUNT,
};

enum L3D_VERTEX_SHADER
{
    L3D_VERTEX_SHADER_CI_MESH,
    L3D_VERTEX_SHADER_CI_SKINMESH,
    L3D_VERTEX_SHADER_CI_TERRAIN,

    L3D_VERTEX_SHADER_COUNT,
};

struct RENDER_STATE_DRAW
{
    struct DRAW_INDEXED
    {
        UINT uIndexCount;
        UINT uStartIndexLocation;
        INT  nBaseVertexLocation;
    } Indexed;
};

struct RENDER_STAGE_INPUT_ASSEMBLER
{
    L3D_INPUT_LAYOUT         eInputLayout;
    D3D11_PRIMITIVE_TOPOLOGY eTopology;

    struct VERTEX_BUFFER
    {
        ID3D11Buffer* piBuffer = nullptr;
        UINT          uStride  = 0;
        UINT          uOffset  = 0;
    } VertexBuffer;

    struct INDEX_BUFFER
    {
        ID3D11Buffer* piBuffer = nullptr;
        DXGI_FORMAT   eFormat  = DXGI_FORMAT_UNKNOWN;
        UINT          uOffset  = 0;
    } IndexBuffer;

    RENDER_STATE_DRAW Draw;
};

enum class RENDER_PASS
{
    COLOR,
    COLORSOFTMASK,
};

struct SCENE_PATH_TABLE
{
    std::filesystem::path dir;
    std::filesystem::path setting;
    std::filesystem::path sceneInfo;
    std::filesystem::path environment;
    std::filesystem::path coverMap;
    std::filesystem::path landscape;

    std::filesystem::path mapName;
};