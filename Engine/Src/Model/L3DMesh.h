#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>

#include "L3DFormat.h"
#include "L3DInterface.h"
#include "L3DFormat.h"

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

struct MESH_DATA
{
    DWORD dwBoneCount;
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

struct _MESH_FILE_DATA
{
    DWORD dwVertexCount;
    DWORD dwFacesCount;
    DWORD dwSubsetCount;
    DWORD dwVertexFVF;

    // point to array, element count is dwVertexCount
    XMFLOAT3* pPos;
    XMFLOAT3* pNormals;
    XMFLOAT4* pDiffuse;
    //XMCOLOR* pDiffuse;
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

class L3DMesh : public ILModel
{
public:
    HRESULT Create(ID3D11Device* piDevice, const char* szFileName);

    DWORD m_dwBoneCount = 0;
    RENDER_STAGE_INPUT_ASSEMBLER m_Stage;

private:
    HRESULT LoadMeshData(const char* szFileName, _MESH_FILE_DATA* pLMeshData);
    HRESULT CreateMesh(const _MESH_FILE_DATA* pLMeshData, ID3D11Device* piDevice);

    HRESULT InitFillInfo(const _MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO* pFillInfo);

    HRESULT InitVertexBuffer(ID3D11Device* piDevice, const _MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO& FillInfo);
    HRESULT InitIndexBuffer(ID3D11Device* piDevice, const _MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO& FillInfo);

    HRESULT InitRenderParam(const VERTEX_FILL_INFO& FillInfo);

    HRESULT FillSkinData(_MESH_FILE_DATA* pMeshData, DWORD nVertexCount);
    HRESULT LoadBoneInfo(MESH_FILE_BONE_INFO* pBoneInfo, BYTE* pbyBone, BOOL bHasPxPose, BOOL bHasBoundBox);

    NormalMesh m_NormalMesh;
};