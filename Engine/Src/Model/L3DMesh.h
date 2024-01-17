#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <string>

#include "L3DFormat.h"
#include "L3DInterface.h"

struct MESH_SOURCE;

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
    L3D_INPUT_LAYOUT eInputLayout;
    NORMAL_MESH_VERTEX_DESC VertexDesc;
    unsigned uVertexSize;
    enum _VERTEX_ADDITIVE_ELEM_TYPE
    {
        NORMAL,
        DIFFUSE,
        UV1,
        UV2,
        UV3,
        SKIN,
        TANGENT,
        COUNT
    };
    unsigned uAdditiveElemId[COUNT];
    unsigned uElementCount;
    struct _ELEM
    {
        unsigned DestStride;
        unsigned SourceStride;
        ::byte* SourceData;
    } Element[COUNT];
};

class L3DBone;
class LBinaryReader;

class L3DMesh
{
public:
    struct _SUBSET
    {
        unsigned uIndexCount = 0;
        unsigned uIndexOffset = 0;
    };

public:
    HRESULT Create(ID3D11Device* piDevice, const char* szFileName);

    void ApplyMeshSubset(RENDER_STAGE_INPUT_ASSEMBLER& State, unsigned int nSubsetIndex);

    std::string m_sName;

    DXGI_FORMAT m_eFormat = DXGI_FORMAT_UNKNOWN;
    L3D_INPUT_LAYOUT m_eInputLayout = L3D_INPUT_LAYOUT_CI_MESH;
    D3D11_PRIMITIVE_TOPOLOGY m_eTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

    ID3D11Buffer* m_piVertexBuffer = nullptr;
    ID3D11Buffer* m_piIndexBuffer  = nullptr;

    std::vector<_SUBSET> m_Subset;
    std::vector<XMMATRIX> m_BoneMatrix; // 初始状态下(T Pose)骨骼的绝对（到原点）变换矩阵

    unsigned int m_nVertexSize  = 0;
    unsigned int m_dwBoneCount  = 0;

    L3DBone* m_pL3DBone = nullptr; // Like KG3D_BoneInfoData

private:
    HRESULT CreateMesh(ID3D11Device* piDevice, const MESH_SOURCE* pSource);

    HRESULT InitVertexBuffer(ID3D11Device* piDevice, const MESH_SOURCE* pSource);
    template<typename _INDEX_TYPE>
    HRESULT InitIndexBuffer(ID3D11Device* piDevice, const MESH_SOURCE* pSource, DXGI_FORMAT eFormat);

    void _CreateBone(const MESH_SOURCE* pSource);
};