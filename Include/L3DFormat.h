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

		char         szName[30];
		unsigned     uChildCount;
		BONE_NAME* pChildNames;
		XMFLOAT4X4   mOffset2Parent;
		XMFLOAT4X4   mOffset;
		XMFLOAT4X4   mInvPxPose;
		unsigned     uRefVertexCount;
		DWORD* pRefVertexIndex;
		float* pRefVertexWeight;
		L3D_BOUND_BOX BoundingBox;
	} *pBone;

	DWORD dwSocketCount;
	struct SOCKET
	{
		char szName[30];
		char szParentName[30];
		XMFLOAT4X4 mOffset;
	} *pSocket;
};

enum L3D_INPUT_LAYOUT
{
    L3D_INPUT_LAYOUT_BOX,
    L3D_INPUT_LAYOUT_CI_SKINMESH,

    L3D_INPUT_LAYOUT_COUNT,
};

enum L3D_SHADER_EFFECT
{
    L3D_SHADER_EFFECT_BOX,
    L3D_SHADER_EFFECT_CI_SKINMESH,

    L3D_SHADER_EFFECT_COUNT,
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
    union
    {
        L3D_INPUT_LAYOUT eInputLayer;
        L3D_SHADER_EFFECT eShaderEffect;
    };
    D3D11_PRIMITIVE_TOPOLOGY eTopology;

    struct VERTEX_BUFFER
    {
        ID3D11Buffer* piBuffer;
        UINT uStride;
        UINT uOffset;
    } VertexBuffer;

    struct INDEX_BUFFER
    {
        ID3D11Buffer* piBuffer;
        DXGI_FORMAT eFormat;
        UINT uOffset;
    } IndexBuffer;

    RENDER_STATE_DRAW Draw;

    BOOL bUseEffect;
};