#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>

#include "L3DRenderUnit.h"

#define D3DFVF_XYZ              0x002
#define D3DFVF_XYZRHW           0x004
#define D3DFVF_XYZB1            0x006
#define D3DFVF_XYZB2            0x008
#define D3DFVF_XYZB3            0x00a
#define D3DFVF_XYZB4            0x00c
#define D3DFVF_XYZB5            0x00e
#define D3DFVF_XYZW             0x4002

#define D3DFVF_NORMAL           0x010
#define D3DFVF_PSIZE            0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080

#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEX2             0x200
#define D3DFVF_TEX3             0x300

#define MAX_NUM_SUBSET_COUNT 32

// TODO
using namespace DirectX;
using namespace DirectX::PackedVector;

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

	static const unsigned MAX_BONE_INFLUNCE = 4;
	struct SKIN
	{
		float BoneWeights[MAX_BONE_INFLUNCE];
		BYTE BoneIndices[MAX_BONE_INFLUNCE];
	} *pSkin;

	DWORD* pIndex;    // element count is dwFacesCount * 3
	// TODO: Subset may be DWORD
	WORD* pSubset;   // element count is dwFacesCount,value in [0, dwSubsetCount - 1]

	MESH_FILE_BONE_INFO BoneInfo;
};

struct NORMAL_MESH_VERTEX_DESC
{
	unsigned bHasPosition : 1;
	unsigned bHasNormal : 1;
	unsigned bHasTangent : 1;
	unsigned bHasDiffuse : 1;
	unsigned bHasSkinInfo : 1;
	unsigned uUVCount : 2;
};

struct VERTEX_FILL_INFO
{
	static const unsigned MAX_ELEMENT = 8;

	L3D_INPUT_LAYOUT eInputLayout;
	NORMAL_MESH_VERTEX_DESC VertexDesc;
	unsigned uVertexSize;
	enum _VERTEX_ADDITIVE_ELEM_TYPE
	{
		VERTEX_ADDITIVE_ELEM_NORMAL,
		VERTEX_ADDITIVE_ELEM_DIFFUSE,
		VERTEX_ADDITIVE_ELEM_UV1,
		VERTEX_ADDITIVE_ELEM_UV2,
		VERTEX_ADDITIVE_ELEM_UV3,
		VERTEX_ADDITIVE_ELEM_SKIN,
		VERTEX_ADDITIVE_ELEM_TANGENT,
		VERTEX_ADDITIVE_ELEM_COUNT
	};
	unsigned uAdditiveElemId[VERTEX_ADDITIVE_ELEM_COUNT];
	unsigned uElementCount;
	struct _ELEM
	{
		unsigned DestStride;
		unsigned SourceStride;
		::byte* SourceData;
	} Element[MAX_ELEMENT];
};


struct NormalMesh
{
	unsigned        uSubsetCount;
	struct _SUBSET
	{
		unsigned    uIndexCount;
		unsigned    uIndexOffset;
		unsigned    uVertexCount;
		unsigned    uVertexOffset;
	} Subset[MAX_NUM_SUBSET_COUNT];
};

class L3DMesh : public L3DRenderUnit
{
public:
	HRESULT LoadMesh(ID3D11Device* piDevice, const char* szFileName);

	HRESULT Draw(ID3D11DeviceContext* pDeviceContext, XMMATRIX* pWorldViewProj) override;
	HRESULT PushRenderUnit(std::vector<RENDER_STAGE_INPUT_ASSEMBLER>& vecRenderUnit) override;

private:
	HRESULT LoadMeshData(const char* szFileName, MESH_FILE_DATA* pLMeshData);
	HRESULT CreateMesh(const MESH_FILE_DATA* pLMeshData, ID3D11Device* piDevice);

	HRESULT InitFillInfo(const MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO* pFillInfo);

	HRESULT InitVertexBuffer(ID3D11Device* piDevice, const MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO& FillInfo);
	HRESULT InitIndexBuffer(ID3D11Device* piDevice, const MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO& FillInfo);

	HRESULT InitRenderParam(const VERTEX_FILL_INFO& FillInfo);

	HRESULT FillSkinData(MESH_FILE_DATA* pMeshData, DWORD nVertexCount);
	HRESULT LoadBoneInfo(MESH_FILE_BONE_INFO* pBoneInfo, BYTE* pbyBone, BOOL bHasPxPose, BOOL bHasBoundBox);

	NormalMesh m_NormalMesh;
};