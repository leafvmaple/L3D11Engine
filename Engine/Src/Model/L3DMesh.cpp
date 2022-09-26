#include "stdafx.h"

#include <algorithm>
#include <map>

#include "L3DMesh.h"

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

    _MESH_FILE_DATA MeshFileData;

    hr = LoadMeshData(szFileName, &MeshFileData);
    HRESULT_ERROR_EXIT(hr);

    hr = CreateMesh(&MeshFileData, piDevice);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT L3DMesh::LoadMeshData(const char* szFileName, _MESH_FILE_DATA* pMeshData)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    
    size_t uMeshLen = 0;
    BYTE* pbyMesh = nullptr;
    BYTE* pbyBufferHead = nullptr;
    _MESH_FILE_HEAD* pHead = nullptr;

    BOOL bBin = false;
    BOOL bHasPxPose = false;

    XMCOLOR* pColor = nullptr;

    ZeroMemory(pMeshData, sizeof(_MESH_FILE_DATA));

    hr = LFileReader::Reader(szFileName, &pbyMesh, &uMeshLen);
    HRESULT_ERROR_EXIT(hr);

    pbyBufferHead = pbyMesh;

    pbyMesh = LFileReader::Convert(pbyMesh, pHead);
    BOOL_ERROR_EXIT(pHead->VersionHead.dwFileMask == _MESH_FILE_VERSION_HEAD::s_dwFileMask);

    pMeshData->dwVertexCount = pHead->MeshHead.dwNumVertices;
    pMeshData->dwFacesCount  = pHead->MeshHead.dwNumFaces;
    pMeshData->dwSubsetCount = pHead->MeshHead.dwNumSubset;
    
    bBin       = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_TOBIN); // TODO
    bHasPxPose = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_ADDITIVE_PX_POSE); // TODO

    // Position
    if (pHead->MeshHead.Blocks.PositionBlock)
    {
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.PositionBlock, pMeshData->pPos, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_XYZ;
    }

    // Normal
    if (pHead->MeshHead.Blocks.NormalBlock)
    {
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.NormalBlock, pMeshData->pNormals, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_NORMAL;
    }

    // Tangent
    if (pHead->MeshHead.Blocks.TangentBlock)
    {
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.TangentBlock, pMeshData->pTangent, pHead->MeshHead.dwNumVertices);
    }

    // Diffuse
    if (pHead->MeshHead.Blocks.DiffuseBlock)
    {
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.DiffuseBlock, pMeshData->pDiffuse, pHead->MeshHead.dwNumVertices);
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
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.TextureUVW1Block, pMeshData->pUV1, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_TEX1;
    }

    // UV2
    if (pHead->MeshHead.Blocks.TextureUVW2Block)
    {
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.TextureUVW2Block, pMeshData->pUV2, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_TEX2;
        pMeshData->dwVertexFVF &= ~(D3DFVF_TEX1);
    }

    // UV3
    if (pHead->MeshHead.Blocks.TextureUVW3Block)
    {
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.TextureUVW3Block, pMeshData->pUV3, pHead->MeshHead.dwNumVertices);
        pMeshData->dwVertexFVF |= D3DFVF_TEX3;
        pMeshData->dwVertexFVF &= ~(D3DFVF_TEX1 | D3DFVF_TEX2);
    }

    // Index
    if (pHead->MeshHead.Blocks.FacesIndexBlock)
    {
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.FacesIndexBlock, pMeshData->pIndex, 3 * pHead->MeshHead.dwNumFaces);
    }

    // Subset Index
    if (pHead->MeshHead.Blocks.SubsetIndexBlock)
    {
        LFileReader::Convert(pbyBufferHead + pHead->MeshHead.Blocks.SubsetIndexBlock, pMeshData->pSubset, pHead->MeshHead.dwNumFaces);
    }

    // Skin Info
    if (pHead->MeshHead.Blocks.SkinInfoBlock)
    {
        hr = LoadBoneInfo(&pMeshData->BoneInfo, pbyBufferHead + pHead->MeshHead.Blocks.SkinInfoBlock, bHasPxPose, pHead->MeshHead.Blocks.BBoxBlock);
        HRESULT_ERROR_EXIT(hr);

        hr = FillSkinData(pMeshData, pMeshData->dwVertexCount);
        HRESULT_ERROR_EXIT(hr);
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMesh::CreateMesh(const _MESH_FILE_DATA* pMeshData, ID3D11Device* piDevice)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    VERTEX_FILL_INFO FillInfo;

    ZeroMemory(&FillInfo, sizeof(FillInfo));

    hr = InitFillInfo(pMeshData, &FillInfo);
    HRESULT_ERROR_EXIT(hr);

    hr = InitVertexBuffer(piDevice, pMeshData, FillInfo);
    HRESULT_ERROR_EXIT(hr);

    hr = InitIndexBuffer(piDevice, pMeshData, FillInfo);
    HRESULT_ERROR_EXIT(hr);

    hr = InitRenderParam(FillInfo);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMesh::InitFillInfo(const _MESH_FILE_DATA* pFileData, VERTEX_FILL_INFO* pFillInfo)
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

HRESULT L3DMesh::InitVertexBuffer(ID3D11Device* piDevice, const _MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO& FillInfo)
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
            VERTEX_FILL_INFO::_ELEM& Elem = FillInfo.Element[j];

            memcpy(pbyOffset, Elem.SourceData, Elem.DestStride);

            Elem.SourceData += Elem.SourceStride;
            pbyOffset += Elem.DestStride;
        }
    }

    desc.ByteWidth = uBufferSize;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    InitData.pSysMem = pbyVertexBuffer;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    hr = piDevice->CreateBuffer(&desc, &InitData, &m_Stage.VertexBuffer.piBuffer);
    HRESULT_ERROR_EXIT(hr);

    m_Stage.VertexBuffer.uStride = FillInfo.uVertexSize;
    m_Stage.VertexBuffer.uOffset = 0;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMesh::InitIndexBuffer(ID3D11Device* piDevice, const _MESH_FILE_DATA* pMeshData, VERTEX_FILL_INFO& FillInfo)
{
    struct _INDEX_DATA
    {
        std::vector<DWORD> IndexBuffer;
        DWORD IndexMin;
        DWORD IndexMax;

        _INDEX_DATA()
        {
            IndexMin = (DWORD)-1;
            IndexMax = 0;
        }
    };

    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    DWORD nCount = 0;
    std::map<int, _INDEX_DATA> IndexBufferList;
    std::vector<WORD> IndexData;

    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA InitData;

    memset(&m_NormalMesh, 0, sizeof(m_NormalMesh));

    for (int i = 0; i < pMeshData->dwFacesCount; i++)
    {
        DWORD nIndex0 = pMeshData->pIndex[i * 3];
        DWORD nIndex1 = pMeshData->pIndex[i * 3 + 1];
        DWORD nIndex2 = pMeshData->pIndex[i * 3 + 2];

        DWORD IndexMin = min(nIndex0, min(nIndex1, nIndex2));
        DWORD IndexMax = max(nIndex0, max(nIndex1, nIndex2));

        auto SubsetIndex = pMeshData->pSubset[i];

        auto& IndexData = IndexBufferList[SubsetIndex];
        IndexData.IndexBuffer.push_back(nIndex0);
        IndexData.IndexBuffer.push_back(nIndex1);
        IndexData.IndexBuffer.push_back(nIndex2);

        IndexData.IndexMin = min(IndexData.IndexMin, IndexMin);
        IndexData.IndexMax = max(IndexData.IndexMax, IndexMax);
    }

    for (auto indexBuff : IndexBufferList)
    {
        for (auto index : indexBuff.second.IndexBuffer)
        {
            IndexData.push_back(static_cast<WORD>(index));
        }
        m_NormalMesh.Subset[nCount++] = { (DWORD)indexBuff.second.IndexBuffer.size(), nCount, indexBuff.second.IndexMax, indexBuff.second.IndexMin };
    }

    desc.ByteWidth = IndexData.size() * sizeof(WORD);
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    InitData.pSysMem = IndexData.data();
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    hr = piDevice->CreateBuffer(&desc, &InitData, &m_Stage.IndexBuffer.piBuffer);
    HRESULT_ERROR_EXIT(hr);

    m_Stage.IndexBuffer.eFormat = DXGI_FORMAT_R16_UINT;
    m_Stage.IndexBuffer.uOffset = 0;

    m_Stage.Draw.Indexed.uIndexCount = IndexData.size();
    m_Stage.Draw.Indexed.nBaseVertexLocation = 0;
    m_Stage.Draw.Indexed.uStartIndexLocation = 0;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMesh::InitRenderParam(const VERTEX_FILL_INFO& FillInfo)
{
    m_Stage.eInputLayer = L3D_INPUT_LAYOUT_CI_SKINMESH;
    m_Stage.eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    return S_OK;
}

HRESULT L3DMesh::FillSkinData(_MESH_FILE_DATA* pMeshData, DWORD nVertexCount)
{
    HRESULT hResult = E_FAIL;

    pMeshData->pSkin = new _MESH_FILE_DATA::SKIN[nVertexCount];
    memset(pMeshData->pSkin, 0xFF, sizeof(pMeshData->pSkin[0]) * nVertexCount);

    for (unsigned i = 0; i < pMeshData->BoneInfo.dwBoneCount; i++)
    {
        MESH_FILE_BONE_INFO::BONE& Bone = pMeshData->BoneInfo.pBone[i];

        for (unsigned j = 0; j < Bone.uRefVertexCount; j++)
        {
            unsigned uVertexIndex = Bone.pRefVertexIndex[j];
            float fWeight = Bone.pRefVertexWeight[j];

            BOOL_ERROR_EXIT(uVertexIndex < nVertexCount);

            float* pBoneWeights = pMeshData->pSkin[uVertexIndex].BoneWeights;
            BYTE* pBoneIndices = pMeshData->pSkin[uVertexIndex].BoneIndices;

            for (unsigned k = 0; k < _MESH_FILE_DATA::MAX_BONE_INFLUNCE; k++)
            {
                if (pBoneIndices[k] == 0xFF)
                {
                    pBoneIndices[k] = (BYTE)i;
                    pBoneWeights[k] = fWeight;
                    break;
                }
            }
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT L3DMesh::LoadBoneInfo(MESH_FILE_BONE_INFO* pBoneInfo, BYTE* pbySkin, BOOL bHasPxPose, BOOL bHasBoundBox)
{
    pbySkin = LFileReader::Copy(pbySkin, &pBoneInfo->dwBoneCount);
    m_dwBoneCount = pBoneInfo->dwBoneCount;

    pBoneInfo->pBone = new MESH_FILE_BONE_INFO::BONE[pBoneInfo->dwBoneCount];

    for (DWORD i = 0; i < pBoneInfo->dwBoneCount; i++)
    {
        MESH_FILE_BONE_INFO::BONE& Bone = pBoneInfo->pBone[i];

        pbySkin = LFileReader::Copy(pbySkin, Bone.szName, 30);
        pbySkin += 30;  // Skip 30 BYTEs

        pbySkin = LFileReader::Convert(pbySkin, Bone.uChildCount);
        Bone.pChildNames = nullptr;
        if (Bone.uChildCount > 0)
        {
            Bone.pChildNames = new MESH_FILE_BONE_INFO::BONE::BONE_NAME[Bone.uChildCount];
            pbySkin = LFileReader::Copy(pbySkin, Bone.pChildNames, Bone.uChildCount);
        }

        pbySkin = LFileReader::Copy(pbySkin, &Bone.mOffset);
        pbySkin = LFileReader::Copy(pbySkin, &Bone.mOffset2Parent);

        if (bHasPxPose)
            pbySkin = LFileReader::Copy(pbySkin, &Bone.mInvPxPose);

        pbySkin = LFileReader::Copy(pbySkin, &Bone.uRefVertexCount);

        if (Bone.uRefVertexCount > 0)
        {
            Bone.pRefVertexIndex = new DWORD[Bone.uRefVertexCount];
            Bone.pRefVertexWeight = new float[Bone.uRefVertexCount];

            pbySkin = LFileReader::Copy(pbySkin, Bone.pRefVertexIndex, Bone.uRefVertexCount);
            pbySkin = LFileReader::Copy(pbySkin, Bone.pRefVertexWeight, Bone.uRefVertexCount);
        }

        if (bHasBoundBox)
        {
            pbySkin += sizeof(XMFLOAT4X4);
            pbySkin = LFileReader::Copy(pbySkin, &Bone.BoundingBox);
            pbySkin += sizeof(BOOL);
        }
    }

    return S_OK;
}
