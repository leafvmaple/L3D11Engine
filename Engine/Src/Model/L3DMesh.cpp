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

    State.eTopology = m_eTopology;
    State.eInputLayout = m_eInputLayout;
    State.VertexBuffer.piBuffer = m_piVertexBuffer;
    State.VertexBuffer.uOffset = 0;
    State.VertexBuffer.uStride = m_nVertexSize;

    State.IndexBuffer.piBuffer = m_piIndexBuffer;
    State.IndexBuffer.uOffset = 0;
    State.IndexBuffer.eFormat = m_eFormat;

    State.Draw.Indexed.uIndexCount = subset.uIndexCount;
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
    
    bBin         = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_TOBIN); // TODO
    bHasPxPose   = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_ADDITIVE_PX_POSE); // TODO
    bSubsetShort = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_SUBSETSHORT);

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
        if (!bSubsetShort)
        {
            Reader.Seek(pHead->MeshHead.Blocks.SubsetIndexBlock);
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

    InitFillInfo(pMeshData, &FillInfo);
    CreateVertexAndIndexBuffer(piDevice, pMeshData, FillInfo);
    CreateBoneInfo(pMeshData->BoneInfo);

    m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    m_nVertexSize = FillInfo.uVertexSize;
    m_eInputLayout = FillInfo.eInputLayout;

Exit0:
    return S_OK;
}

HRESULT L3DMesh::InitFillInfo(const MESH_FILE_DATA* pFileData, VERTEX_FILL_INFO* pFillInfo)
{
    HRESULT hResult = E_FAIL;

    unsigned uElemId = 0;
    unsigned uSourceStride = 0;
    VERTEX_FILL_INFO::_ELEM* pElem = nullptr;

    pElem = &pFillInfo->Element[0];
    uSourceStride = sizeof(XMFLOAT3);
    pElem->DestStride = sizeof(XMFLOAT3);
    pElem->SourceStride = uSourceStride;
    pElem->SourceData = (byte*)pFileData->pPos;

    pFillInfo->VertexDesc.bHasPosition = true;
    pFillInfo->uVertexSize = pElem->DestStride;

    // Auto convert XMFLOAT3->float4
    if (pFileData->pNormals)
    {
        ++pElem;
        uElemId++;
        uSourceStride = sizeof(XMFLOAT3);
        pElem->DestStride = sizeof(XMFLOAT3);
        pElem->SourceStride = uSourceStride;
        pElem->SourceData = (byte*)pFileData->pNormals;

        pFillInfo->VertexDesc.bHasNormal = true;
        pFillInfo->uVertexSize += pElem->DestStride;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::VERTEX_ADDITIVE_ELEM_NORMAL] = uElemId;
    }

    // Auto convert XMCOLOR->float4
    if (pFileData->pDiffuse)
    {
        ++pElem;
        uElemId++;
        uSourceStride = sizeof(XMCOLOR);
        pElem->DestStride = sizeof(XMCOLOR);
        pElem->SourceStride = uSourceStride;
        pElem->SourceData = (byte*)pFileData->pDiffuse;

        pFillInfo->VertexDesc.bHasDiffuse = true;
        pFillInfo->uVertexSize += pElem->DestStride;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::VERTEX_ADDITIVE_ELEM_DIFFUSE] = uElemId;
    }

    // TEXCOORD0 XMFLOAT3->float2
    if (pFileData->pUV1)
    {
        ++pElem;
        uElemId++;
        uSourceStride = sizeof(XMFLOAT3);
        pElem->DestStride = sizeof(XMFLOAT2);
        pElem->SourceStride = uSourceStride;
        pElem->SourceData = (byte*)pFileData->pUV1;

        pFillInfo->VertexDesc.uUVCount = 1;
        pFillInfo->uVertexSize += pElem->DestStride;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::VERTEX_ADDITIVE_ELEM_UV1] = uElemId;
    }

    if (pFileData->pUV2)
    {
        ++pElem;
        uElemId++;
        uSourceStride = sizeof(XMFLOAT3);
        pElem->DestStride = sizeof(XMFLOAT2);
        pElem->SourceStride = uSourceStride;
        pElem->SourceData = (byte*)pFileData->pUV2;

        pFillInfo->VertexDesc.uUVCount = 2;
        pFillInfo->uVertexSize += pElem->DestStride;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::VERTEX_ADDITIVE_ELEM_UV2] = uElemId;
    }

    if (pFileData->pUV3)
    {
        ++pElem;
        uElemId++;
        uSourceStride = sizeof(XMFLOAT3);
        pElem->DestStride = sizeof(XMFLOAT2);
        pElem->SourceStride = uSourceStride;
        pElem->SourceData = (byte*)pFileData->pUV3;

        pFillInfo->VertexDesc.uUVCount = 3;
        pFillInfo->uVertexSize += pElem->DestStride;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::VERTEX_ADDITIVE_ELEM_UV3] = uElemId;
    }

    // BONEWEIGHTS0 && BONEINDICES0
    if (pFileData->pSkin)
    {
        BOOL_ERROR_EXIT(pFileData->pNormals);
        BOOL_ERROR_EXIT(pFileData->pUV1);

        ++pElem;
        uElemId++;
        uSourceStride = sizeof(pFileData->pSkin[0]);
        pElem->DestStride = uSourceStride;
        pElem->SourceStride = uSourceStride;
        pElem->SourceData = (byte*)pFileData->pSkin;

        pFillInfo->VertexDesc.bHasSkinInfo = true;
        pFillInfo->uVertexSize += pElem->DestStride;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::VERTEX_ADDITIVE_ELEM_SKIN] = uElemId;
    }

    if (pFileData->pTangent)
    {
        BOOL_ERROR_EXIT(pFileData->pNormals);

        ++pElem;
        uElemId++;
        pElem->DestStride = sizeof(XMFLOAT4);
        pElem->SourceStride = sizeof(XMFLOAT4);
        pElem->SourceData = (byte*)pFileData->pTangent;

        pFillInfo->VertexDesc.bHasTangent = true;
        pFillInfo->uVertexSize += pElem->DestStride;
        pFillInfo->uAdditiveElemId[VERTEX_FILL_INFO::VERTEX_ADDITIVE_ELEM_TANGENT] = uElemId;
    }

    if (pFileData->pTangent)
    {
        if (pFileData->pSkin)
        {
            if (pFileData->pUV3) {}
            //pRetFillInfo->eInputLayout = L3D_DYNAMIC_INPUT_LAYOUT_CI_SKINMESH_UV3;
            else if (pFileData->pUV2) {}
            //pRetFillInfo->eInputLayout = L3D_DYNAMIC_INPUT_LAYOUT_CI_SKINMESH_UV2;
            else if (pFileData->pUV1)
                pFillInfo->eInputLayout = L3D_INPUT_LAYOUT_CI_SKINMESH;
            else
                assert(false);
        }
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
        std::vector<DWORD> IndexBuffer;
        DWORD IndexMin = std::numeric_limits<DWORD>::max();
        DWORD IndexMax = 0;
    };

    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    DWORD nTotalCount = 0;
    DWORD nSubIndex = 0;
    std::map<int, _INDEX_DATA> IndexBufferList;
    std::vector<_INDEX_TYPE> arrIndies;

    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA InitData;

    for (int i = 0; i < pMeshData->dwFacesCount; i++)
    {
        DWORD nIndex0 = pMeshData->pIndex[i * 3];
        DWORD nIndex1 = pMeshData->pIndex[i * 3 + 1];
        DWORD nIndex2 = pMeshData->pIndex[i * 3 + 2];

        DWORD IndexMin = std::min(nIndex0, std::min(nIndex1, nIndex2));
        DWORD IndexMax = std::max(nIndex0, std::max(nIndex1, nIndex2));

        auto SubsetIndex = pMeshData->pSubset[i];

        auto& IndexData = IndexBufferList[SubsetIndex];
        IndexData.IndexBuffer.push_back(nIndex0);
        IndexData.IndexBuffer.push_back(nIndex1);
        IndexData.IndexBuffer.push_back(nIndex2);

        IndexData.IndexMin = std::min(IndexData.IndexMin, IndexMin);
        IndexData.IndexMax = std::max(IndexData.IndexMax, IndexMax);
    }

    assert(IndexBufferList.size() == pMeshData->dwSubsetCount);

    for (auto indexBuff : IndexBufferList)
    {
        DWORD nCount = indexBuff.second.IndexBuffer.size();

        for (auto index : indexBuff.second.IndexBuffer)
            arrIndies.push_back(static_cast<_INDEX_TYPE>(index));

        m_Subset.push_back({ (DWORD)nCount, nTotalCount, indexBuff.second.IndexMin, indexBuff.second.IndexMax - indexBuff.second.IndexMin + 1 });
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

void L3DMesh::CreateBoneInfo(const MESH_FILE_BONE_INFO& BoneInfo)
{
    m_pL3DBone = new(std::nothrow) L3DBone;
    m_pL3DBone->BindData(BoneInfo);

    m_dwBoneCount = BoneInfo.dwBoneCount;

    m_BoneMatrix.resize(m_dwBoneCount);
    for (unsigned int i = 0; i < m_dwBoneCount; i++)
        m_BoneMatrix[i] = XMMatrixInverse(NULL, m_pL3DBone->m_pBoneInfo->BoneOffset[i]);
}
