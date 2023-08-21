#pragma once

#include <d3d11.h>
#include <DirectXPackedVector.h>

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

struct _MESH_FILE_COMON_HEAD
{
    char    szCommonTag[8];
    char    szModuleTag[8];
    __int32 byVersion;
    char    szDescription[64];
};

struct _MESH_FILE_VERSION_HEAD
{
    static const DWORD s_dwFileMask = 0x4d455348; // "mesh"
    static const DWORD s_dwCurrentVersion = 2;
    static const DWORD s_dwDeltaVersionMeshToBin = 100000;

    _MESH_FILE_COMON_HEAD KSFileHeader_NotUse;
    DWORD       dwFileMask;
    DWORD       dwBlockLength;
    DWORD       dwVersion;
    DWORD       AnimationBlock_NotUse;
    DWORD       ExtendBlock_NotUse[10];
    DWORD       MeshCount_NotUse;
};

struct _MESH_FILE_DATA_BLOCKS
{
    DWORD  PositionBlock;
    DWORD  NormalBlock;
    DWORD  DiffuseBlock;
    DWORD  TextureUVW1Block;
    DWORD  TextureUVW2Block;
    DWORD  TextureUVW3Block;
    DWORD  FacesIndexBlock;
    DWORD  SubsetIndexBlock;
    DWORD  SkinInfoBlock;
    DWORD  LODInfoBlock;
    DWORD  FlexibleBodyBlock; // not need the data now
    DWORD  BBoxBlock;
    DWORD  BlendMeshBlock;
    DWORD  TangentBlock;
    DWORD  VetexRemapBlock;
    DWORD  ExtendBlock[15];
};

struct _MESH_FILE_MESH_HEAD
{
    DWORD  dwNumVertices;
    DWORD  dwNumFaces;
    DWORD  dwNumSubset;
    union
    {
        _MESH_FILE_DATA_BLOCKS Blocks;
        DWORD dwBlocks[30];
    };
};

struct _MESH_FILE_HEAD
{
    _MESH_FILE_VERSION_HEAD VersionHead;
    _MESH_FILE_MESH_HEAD MeshHead;
};

typedef struct L3D_BOUND_BOX
{
    float fMinX, fMinY, fMinZ;
    float fMaxX, fMaxY, fMaxZ;
} L3D_BOUND_BOX;

struct MESH_FILE_BONE_INFO
{
    DWORD dwBoneCount;
    struct BONE
    {
        typedef char BONE_NAME[30];

        char         szName[30]  = { 0 };
        unsigned     uChildCount = 0;
        BONE_NAME*   pChildNames = nullptr;
        XMFLOAT4X4   mOffset2Parent;
        XMFLOAT4X4   mOffset;
        XMFLOAT4X4   mInvPxPose;
        unsigned     uRefVertexCount = 0;
        DWORD* pRefVertexIndex       = nullptr;
        float* pRefVertexWeight      = nullptr;
        L3D_BOUND_BOX BoundingBox;
    } *pBone = nullptr;

    DWORD dwSocketCount;
    struct SOCKET
    {
        char szName[30]       = { 0 };
        char szParentName[30] = { 0 };
        XMFLOAT4X4 mOffset;
    } *pSocket = nullptr;
};

static const unsigned MAX_BONE_INFLUNCE = 4;
struct SKIN
{
    float BoneWeights[MAX_BONE_INFLUNCE];
    BYTE BoneIndices[MAX_BONE_INFLUNCE];
};

struct MESH_FILE_DATA
{
    DWORD dwVertexCount;
    DWORD dwFacesCount;
    DWORD dwSubsetCount;
    DWORD dwVertexFVF;

    // point to array, element count is dwVertexCount
    XMFLOAT3* pPos;
    XMFLOAT3* pNormals;
    XMCOLOR* pDiffuse;
    XMFLOAT3* pUV1;
    XMFLOAT3* pUV2;
    XMFLOAT3* pUV3;
    XMFLOAT4* pTangent;
    DWORD* pVertexRemap;

    SKIN* pSkin;

    DWORD* pIndex;    // element count is dwFacesCount * 3
    // TODO: Subset may be DWORD
    DWORD* pSubset;   // element count is dwFacesCount,value in [0, dwSubsetCount - 1]

    MESH_FILE_BONE_INFO BoneInfo;
};

enum L3D_INPUT_LAYOUT
{
    L3D_INPUT_LAYOUT_BOX,
    L3D_INPUT_LAYOUT_CI_MESH,
    L3D_INPUT_LAYOUT_CI_SKINMESH,

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
    L3D_VERTEX_SHADER_BOX,
    L3D_VERTEX_SHADER_CI_MESH,
    L3D_VERTEX_SHADER_CI_SKINMESH,

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
    char szDir[MAX_PATH]         = { 0 };
    char szSetting[MAX_PATH]     = { 0 };
    char szSceneInfo[MAX_PATH]   = { 0 };
    char szEnvironment[MAX_PATH] = { 0 };

    char szCoverMap[MAX_PATH]    = { 0 };
};