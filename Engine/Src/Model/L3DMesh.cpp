#include "stdafx.h"

#include <algorithm>
#include <map>

#include "L3DMesh.h"

#include "L3DBone.h"
#include "L3DMaterial.h"
#include "IO/LFileReader.h"

enum VersionNewBit
{
    VERSION_BIT_NEWBIT = 0,
    VERSION_BIT_TOBIN,
    VERSION_BIT_SUBSETSHORT,
    VERSION_BIT_ZFP,
    VERSION_BIT_UVNOCOMPRESS,
    VERSION_BIT_ADDITIVE_PX_POSE,

    VERSION_BIT_MAX
};

HRESULT L3DMesh::Create(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    MESH_FILE_DATA MeshFileData;

    hr = LoadMeshData(szFileName, &MeshFileData);
    HRESULT_ERROR_EXIT(hr);

    hr = CreateMesh(&MeshFileData, piDevice);
    HRESULT_ERROR_EXIT(hr);

    m_sName = szFileName;

    hResult = S_OK;
Exit0:
    return hResult;
}


void L3DMesh::ApplyMeshSubset(RENDER_STAGE_INPUT_ASSEMBLER& State, unsigned int nSubsetIndex)
{
    auto& subset = m_Subset[nSubsetIndex];

    State.eTopology    = m_eTopology;
    State.eInputLayout = m_eInputLayout;

    State.VertexBuffer.piBuffer = m_piVertexBuffer;
    State.VertexBuffer.uOffset  = 0;
    State.VertexBuffer.uStride  = m_nVertexSize;

    State.IndexBuffer.piBuffer = m_piIndexBuffer;
    State.IndexBuffer.uOffset  = 0;
    State.IndexBuffer.eFormat  = m_eFormat;

    State.Draw.Indexed.uIndexCount         = subset.uIndexCount;
    State.Draw.Indexed.uStartIndexLocation = subset.uIndexOffset;
    State.Draw.Indexed.nBaseVertexLocation = 0;
}

HRESULT L3DMesh::LoadMeshData(const char* szFileName, MESH_FILE_DATA* pMeshData)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    
    _MESH_FILE_HEAD* pHead = nullptr;

    BOOL bBin = false;
    BOOL bHasPxPose = false;
    BOOL bSubsetShort = false;

    LBinaryReader Reader;

    ZeroMemory(pMeshData, sizeof(MESH_FILE_DATA));

    Reader.Init(szFileName);
    Reader.Convert(pHead);
    assert(pHead->VersionHead.dwFileMask == _MESH_FILE_VERSION_HEAD::s_dwFileMask);

    pMeshData->dwVertexCount = pHead->MeshHead.dwNumVertices;
    pMeshData->dwFacesCount  = pHead->MeshHead.dwNumFaces;
    pMeshData->dwSubsetCount = pHead->MeshHead.dwNumSubset;
    
    if (pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_NEWBIT))
    {
        bBin = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_TOBIN); // TODO
        bHasPxPose = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_ADDITIVE_PX_POSE); // TODO
        bSubsetShort = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_SUBSETSHORT);
    }
    else
    {
        assert(pHead->VersionHead.dwVersion < _MESH_FILE_VERSION_HEAD::s_dwDeltaVersionMeshToBin);

        bBin = false;
        bSubsetShort = pHead->VersionHead.dwVersion >= 1;
        bHasPxPose = pHead->VersionHead.dwVersion >= 2;
    }

    // Position
    if (pHead->MeshHead.Blocks.PositionBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.PositionBlock);
        Reader.Convert(pMeshData->pPos, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_XYZ;
    }

    // Normal
    if (pHead->MeshHead.Blocks.NormalBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.NormalBlock);
        Reader.Convert(pMeshData->pNormals, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_NORMAL;
    }

    // Tangent
    if (pHead->MeshHead.Blocks.TangentBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.TangentBlock);
        Reader.Convert(pMeshData->pTangent, pHead->MeshHead.dwNumVertices);
    }
    else
    {
        pMeshData->pTangent = new XMFLOAT4[pHead->MeshHead.dwNumVertices];
        for (int i = 0; i < pHead->MeshHead.dwNumVertices; i++)
            pMeshData->pTangent[i] = XMFLOAT4(.0f, 1.0f, 0.0f, 1.0f);
    }

    // Diffuse
    if (pHead->MeshHead.Blocks.DiffuseBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.DiffuseBlock);
        Reader.Convert(pMeshData->pDiffuse, pHead->MeshHead.dwNumVertices);
    }
    else
    {
        pMeshData->pDiffuse = new XMCOLOR[pHead->MeshHead.dwNumVertices];
        for (int i = 0; i < pHead->MeshHead.dwNumVertices; i++)
            pMeshData->pDiffuse[i] = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    }
    pMeshData->dwVertexFVF |= D3DFVF_DIFFUSE;

    // UV1
    if (pHead->MeshHead.Blocks.TextureUVW1Block)
    {
        Reader.Seek(pHead->MeshHead.Blocks.TextureUVW1Block);
        Reader.Convert(pMeshData->pUV1, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_TEX1;
    }

    // UV2
    if (pHead->MeshHead.Blocks.TextureUVW2Block)
    {
        Reader.Seek(pHead->MeshHead.Blocks.TextureUVW2Block);
        Reader.Convert(pMeshData->pUV2, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_TEX2;
        pMeshData->dwVertexFVF &= ~(D3DFVF_TEX1);
    }

    // UV3
    if (pHead->MeshHead.Blocks.TextureUVW3Block)
    {
        Reader.Seek(pHead->MeshHead.Blocks.TextureUVW3Block);
        Reader.Convert(pMeshData->pUV3, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_TEX3;
        pMeshData->dwVertexFVF &= ~(D3DFVF_TEX1 | D3DFVF_TEX2);
    }

    // Index
    if (pHead->MeshHead.Blocks.FacesIndexBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.FacesIndexBlock);
        Reader.Convert(pMeshData->pIndex, pHead->MeshHead.dwNumFaces);
    }

    // Subset Index
    if (pHead->MeshHead.Blocks.SubsetIndexBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.SubsetIndexBlock);
        if (bSubsetShort)
        {
            WORD* pSubset = nullptr;
            Reader.Convert(pSubset, pHead->MeshHead.dwNumFaces);
            // TODO GC
            pMeshData->pSubset = new DWORD[pHead->MeshHead.dwNumFaces];
            for (int i = 0; i < pHead->MeshHead.dwNumFaces; i++)
            {
                pMeshData->pSubset[i] = static_cast<DWORD>(pSubset[i]);
            }
        }
        else
        {
            Reader.Convert(pMeshData->pSubset, pHead->MeshHead.dwNumFaces);
        }
    }

    // Skin Info
    if (pHead->MeshHead.Blocks.SkinInfoBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.SkinInfoBlock);
        L3DBone::Load(pMeshData->BoneInfo, Reader, bHasPxPose, pHead->MeshHead.Blocks.BBoxBlock);
        L3DBone::FillSkinData(pMeshData->pSkin, pMeshData->BoneInfo, pMeshData->dwVertexCount);
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMesh::CreateMesh(const MESH_FILE_DATA* pMeshData, ID3D11Device* piDevice)
{
    VERTEX_FILL_INFO FillInfo;

    ZeroMemory(&FillInfo, sizeof(FillInfo));

    VertexFillInfo(pMeshData, &FillInfo);
    CreateVertexAndIndexBuffer(piDevice, pMeshData, FillInfo);
    _CreateBoneInfo(pMeshData->BoneInfo);

    m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    m_nVertexSize = FillInfo.uVertexSize;
    m_eInputLayout = FillInfo.eInputLayout;

Exit0:
    return S_OK;
}

// Helper function to fill VERTEX_FILL_INFO::_ELEM
void FillElem(VERTEX_FILL_INFO::_ELEM*& pElem, unsigned& uElemId, unsigned uSourceStride, unsigned uDestStride, byte* sourceData, unsigned& uVertexSize)
{
    pElem->DestStride = uDestStride;
    pElem->SourceStride = uSourceStride;
    pElem->SourceData = sourceData;

    uVertexSize += uDestStride;
    ++pElem;
    ++uElemId;
}

void FillElem(VERTEX_FILL_INFO::_ELEM*& pElem, unsigned& uElemId, unsigned uStride, byte* sourceData, unsigned& uVertexSize)
{
    FillElem(pElem, uElemId, uStride, uStride, sourceData, uVertexSize);
}

// KG3D_GetVertexFillInfo
HRESULT L3DMesh::VertexFillInfo(const MESH_FILE_DATA* pFileData, VERTEX_FILL_INFO* pFillInfo)
{
    HRESULT hResult = E_FAIL;

    unsigned uElemId = 0;
    unsigned uSourceStride = 0;
    VERTEX_FILL_INFO::_ELEM* pElem = nullptr;

    pElem = &pFillInfo->Element[0];
    pFillInfo->VertexDesc.bHasPosition = true;
    FillElem(pElem, uElemId, sizeof(XMFLOAT3), (byte*)pFileData->pPos, pFillInfo->uVertexSize);

    // Auto convert XMFLOAT3->float4
    if (pFileData->pNormals)
    {
        pFillInfo->VertexDesc.bHasNormal = true;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::NORMAL] = uElemId;
        FillElem(pElem, uElemId, sizeof(XMFLOAT3), (byte*)pFileData->pNormals, pFillInfo->uVertexSize);
    }

    // Auto convert XMCOLOR->float4
    if (pFileData->pDiffuse)
    {
        pFillInfo->VertexDesc.bHasDiffuse = true;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::DIFFUSE] = uElemId;
        FillElem(pElem, uElemId, sizeof(XMCOLOR), (byte*)pFileData->pDiffuse, pFillInfo->uVertexSize);
    }

    // TEXCOORD0 XMFLOAT3->float2
    if (pFileData->pUV1)
    {
        pFillInfo->VertexDesc.uUVCount = 1;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::UV1] = uElemId;
        FillElem(pElem, uElemId, sizeof(XMFLOAT3), sizeof(XMFLOAT2), (byte*)pFileData->pUV1, pFillInfo->uVertexSize);
    }

    if (pFileData->pUV2)
    {
        pFillInfo->VertexDesc.uUVCount = 2;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::UV2] = uElemId;
        FillElem(pElem, uElemId, sizeof(XMFLOAT3), sizeof(XMFLOAT2), (byte*)pFileData->pUV2, pFillInfo->uVertexSize);
    }

    if (pFileData->pUV3)
    {
        pFillInfo->VertexDesc.uUVCount = 3;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::UV3] = uElemId;
        FillElem(pElem, uElemId, sizeof(XMFLOAT3), sizeof(XMFLOAT2), (byte*)pFileData->pUV3, pFillInfo->uVertexSize);
    }

    // BONEWEIGHTS0 && BONEINDICES0
    if (pFileData->pSkin)
    {
        BOOL_ERROR_EXIT(pFileData->pNormals);
        BOOL_ERROR_EXIT(pFileData->pUV1);

        pFillInfo->VertexDesc.bHasSkinInfo = true;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::SKIN] = uElemId;
        FillElem(pElem, uElemId, sizeof(pFileData->pSkin[0]), (byte*)pFileData->pSkin, pFillInfo->uVertexSize);
    }

    if (pFileData->pTangent)
    {
        BOOL_ERROR_EXIT(pFileData->pNormals);

        pFillInfo->VertexDesc.bHasTangent = true;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::TANGENT] = uElemId;
        FillElem(pElem, uElemId, sizeof(XMFLOAT4), (byte*)pFileData->pTangent, pFillInfo->uVertexSize);
    }

    if (pFileData->pTangent)
    {
        if (pFileData->pUV3 || pFileData->pUV2)
            assert(false);
        else if (pFileData->pUV1)
            pFillInfo->eInputLayout = pFileData->pSkin ? L3D_INPUT_LAYOUT_CI_SKINMESH : L3D_INPUT_LAYOUT_CI_MESH;
        else
            assert(false);
    }
    else
        pFillInfo->eInputLayout = L3D_INPUT_LAYOUT_BOX;

    pFillInfo->uElementCount = (unsigned)(pElem - &(pFillInfo->Element[0])) + 1;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMesh::InitVertexBuffer(ID3D11Device* piDevice, const MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO& FillInfo)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    BYTE* pbyOffset = nullptr;
    BYTE* pbyVertexBuffer = nullptr;
    DWORD uBufferSize = 0;

    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA InitData;

    uBufferSize = FillInfo.uVertexSize * pMeshData->dwVertexCount;
    pbyVertexBuffer = new BYTE[uBufferSize];
    pbyOffset = pbyVertexBuffer;
    for (int i = 0; i < pMeshData->dwVertexCount; i++)
    {
        for (int j = 0; j < FillInfo.uElementCount; j++)
        {
            auto& Elem = FillInfo.Element[j];

            memcpy(pbyOffset, Elem.SourceData, Elem.DestStride);
            Elem.SourceData += Elem.SourceStride;
            pbyOffset += Elem.DestStride;
        }
    }

    desc.ByteWidth           = uBufferSize;
    desc.Usage               = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags      = 0;
    desc.MiscFlags           = 0;
    desc.StructureByteStride = 0;

    InitData.pSysMem          = pbyVertexBuffer;
    InitData.SysMemPitch      = 0;
    InitData.SysMemSlicePitch = 0;

    hr = piDevice->CreateBuffer(&desc, &InitData, &m_piVertexBuffer);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

template<typename _INDEX_TYPE>
HRESULT L3DMesh::InitIndexBuffer(ID3D11Device* piDevice, const MESH_FILE_DATA* pMeshData, DXGI_FORMAT eFormat)
{
    struct _INDEX_DATA
    {
        std::vector<DWORD> Indies;
        DWORD nMinIndex = std::numeric_limits<DWORD>::max();
        DWORD nMaxIndex = 0;
    };

    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    DWORD nTotalCount = 0;
    std::map<int, _INDEX_DATA> IndexBufferList;
    std::vector<_INDEX_TYPE> arrIndies;

    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA InitData;

    arrIndies.reserve(pMeshData->dwFacesCount * 3); //‘§∑÷≈‰ø’º‰

    for (int i = 0; i < pMeshData->dwFacesCount; i++)
    {
        DWORD nIndex0 = pMeshData->pIndex[i * 3];
        DWORD nIndex1 = pMeshData->pIndex[i * 3 + 1];
        DWORD nIndex2 = pMeshData->pIndex[i * 3 + 2];

        auto& IndexData = IndexBufferList[pMeshData->pSubset[i]];
        IndexData.Indies.emplace_back(nIndex0);
        IndexData.Indies.emplace_back(nIndex1);
        IndexData.Indies.emplace_back(nIndex2);

        IndexData.nMinIndex = std::min({IndexData.nMinIndex, nIndex0, nIndex1, nIndex2});
        IndexData.nMaxIndex = std::max({IndexData.nMaxIndex, nIndex0, nIndex1, nIndex2});
    }

    assert(IndexBufferList.size() == pMeshData->dwSubsetCount);

    for (auto subset : IndexBufferList)
    {
        DWORD nCount = subset.second.Indies.size();

        for (auto index : subset.second.Indies)
            arrIndies.emplace_back(static_cast<_INDEX_TYPE>(index));

        m_Subset.emplace_back(_SUBSET{ (DWORD)nCount, nTotalCount, subset.second.nMinIndex, subset.second.nMaxIndex - subset.second.nMinIndex + 1 });
        nTotalCount += nCount;
    }

    desc.ByteWidth           = arrIndies.size() * sizeof(_INDEX_TYPE);
    desc.Usage               = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags      = 0;
    desc.MiscFlags           = 0;
    desc.StructureByteStride = 0;

    InitData.pSysMem          = arrIndies.data();
    InitData.SysMemPitch      = 0;
    InitData.SysMemSlicePitch = 0;

    hr = piDevice->CreateBuffer(&desc, &InitData, &m_piIndexBuffer);
    HRESULT_ERROR_EXIT(hr);

    m_eFormat = eFormat;

    hResult = S_OK;
Exit0:
    return hResult;
}


void L3DMesh::CreateVertexAndIndexBuffer(ID3D11Device* piDevice, const MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO& FillInfo)
{
    InitVertexBuffer(piDevice, pMeshData, FillInfo);
    InitIndexBuffer<WORD>(piDevice, pMeshData, DXGI_FORMAT_R16_UINT);
}

void L3DMesh::_CreateBoneInfo(const MESH_FILE_BONE_INFO& BoneInfo)
{
    m_pL3DBone = new(std::nothrow) L3DBone;
    m_pL3DBone->BindData(BoneInfo);

    m_dwBoneCount = BoneInfo.dwBoneCount;

    m_BoneMatrix.resize(m_dwBoneCount);
    for (unsigned int i = 0; i < m_dwBoneCount; i++)
        m_BoneMatrix[i] = XMMatrixInverse(nullptr, m_pL3DBone->m_BoneOffset[i]);
}
