#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <string>

#include "L3DFormat.h"
#include "L3DInterface.h"

struct MESH_SOURCE;

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
    bool Create(ID3D11Device* piDevice, const char* szFileName);

    void ApplyMeshSubset(RENDER_STAGE_INPUT_ASSEMBLER& State, unsigned int nSubsetIndex);

    std::string m_sName;

    DXGI_FORMAT m_eFormat = DXGI_FORMAT_UNKNOWN;
    L3D_INPUT_LAYOUT m_nInputLayout = L3D_INPUT_LAYOUT_CI_MESH;
    D3D11_PRIMITIVE_TOPOLOGY m_nTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

    ID3D11Buffer* m_piVertexBuffer = nullptr;
    ID3D11Buffer* m_piIndexBuffer  = nullptr;

    std::vector<_SUBSET> m_Subset;
    std::vector<XMMATRIX> m_BoneMatrix; // 初始状态下(T Pose)骨骼的绝对（到原点）变换矩阵

    unsigned int m_nVertexSize  = 0;
    unsigned int m_dwBoneCount  = 0;

    L3DBone* m_pL3DBone = nullptr; // Like KG3D_BoneInfoData

private:
    bool _CreateMesh(ID3D11Device* piDevice, const MESH_SOURCE* pSource);
    bool _CreateBone(const MESH_SOURCE* pSource);

    bool _CreateVertexBuffer(ID3D11Device* piDevice, const MESH_SOURCE* pSource);
    template<typename T>
    bool _CreateIndexBuffer(ID3D11Device* piDevice, const MESH_SOURCE* pSource, DXGI_FORMAT eFormat);

};