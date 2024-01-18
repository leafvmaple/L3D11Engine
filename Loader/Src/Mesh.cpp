#include "Mesh.h"
#include "LFileReader.h"

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

void _FillSubset(MESH_SOURCE* pSource, _MESH_FILE_DATA* pData)
{
    std::map<int, std::vector<DWORD>> IndexMap;

    for (int i = 0; i < pSource->nIndexCount; i++)
        IndexMap[pData->SubsetIndices[i / 3]].emplace_back(pSource->pIndices[i]);

    assert(pSource->nSubsetCount == IndexMap.size());

    int nOffset = 0;
    pSource->pSubsetVertexCount = new WORD[pSource->nSubsetCount];
    for (auto [nSubsetID, Indices] : IndexMap)
    {
        memcpy(pSource->pIndices + nOffset, Indices.data(), Indices.size() * sizeof(DWORD));
        pSource->pSubsetVertexCount[nSubsetID] = Indices.size();
        nOffset += Indices.size();
    }
}

template<typename T>
static void _LoadSubsetIndex(_MESH_FILE_DATA* pData, LBinaryReader* pReader, int FacesCount)
{
    T* pSubset = nullptr;
    pReader->Convert(pSubset, FacesCount);

    pData->SubsetIndices.resize(FacesCount);
    for (int i = 0; i < FacesCount; i++)
        pData->SubsetIndices[i] = static_cast<T>(pSubset[i]);
}

void _LoadMesh(const char* szFileName, MESH_SOURCE* pSource, _MESH_FILE_DATA* pData)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    _MESH_FILE_HEAD* pHead = nullptr;

    XMFLOAT4* pFloat4 = nullptr;
    XMFLOAT3* pFloat3 = nullptr;
    XMCOLOR* pColor = nullptr;

    BOOL bBin = false;
    BOOL bHasPxPose = false;
    BOOL bSubsetShort = false;

    LBinaryReader Reader;

    Reader.Init(szFileName);
    Reader.Convert(pHead);
    assert(pHead->VersionHead.dwFileMask == _MESH_FILE_VERSION_HEAD::s_dwFileMask);

    pSource->nVerticesCount = pHead->MeshHead.dwNumVertices;
    pSource->nFacesCount = pHead->MeshHead.dwNumFaces;
    pSource->nSubsetCount = pHead->MeshHead.dwNumSubset;
    pSource->nIndexCount = pSource->nFacesCount * 3;

    if (pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_NEWBIT))
    {
        bBin = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_TOBIN);
        bHasPxPose = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_ADDITIVE_PX_POSE);
        bSubsetShort = pHead->VersionHead.dwVersion & (0x80000000 >> VERSION_BIT_SUBSETSHORT);
    }
    else
    {
        assert(pHead->VersionHead.dwVersion < _MESH_FILE_VERSION_HEAD::s_dwDeltaVersionMeshToBin);

        bBin = false;
        bSubsetShort = pHead->VersionHead.dwVersion >= 1;
        bHasPxPose = pHead->VersionHead.dwVersion >= 2;
    }

    pSource->pVertices = new VERTEX_SOURCE[pSource->nVerticesCount];

    // Position
    if (pHead->MeshHead.Blocks.PositionBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.PositionBlock);
        Reader.Convert(pFloat3, pSource->nVerticesCount);
        for (int i = 0; i < pSource->nVerticesCount; i++)
            memcpy(&pSource->pVertices[i].Position, &pFloat3[i], sizeof(XMFLOAT3));

        pSource->nVertexFVF |= FVF_XYZW;
        pSource->nVertexSize += sizeof(XMFLOAT3);
    }

    // Normal
    if (pHead->MeshHead.Blocks.NormalBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.NormalBlock);
        Reader.Convert(pFloat3, pSource->nVerticesCount);
        for (int i = 0; i < pSource->nVerticesCount; i++)
            pSource->pVertices[i].Normal = pFloat3[i];

        pSource->nVertexFVF |= FVF_NORMAL;
        pSource->nVertexSize += sizeof(XMFLOAT3);
    }

    // Tangent
    if (pHead->MeshHead.Blocks.TangentBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.TangentBlock);
        Reader.Convert(pFloat4, pSource->nVerticesCount);
        for (int i = 0; i < pSource->nVerticesCount; i++)
            pSource->pVertices[i].Tangent = pFloat4[i];
    }
    pSource->nVertexFVF |= FVF_TANGENT;
    pSource->nVertexSize += sizeof(XMFLOAT4);

    // Color: XMCOLOR->float4
    if (pHead->MeshHead.Blocks.DiffuseBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.DiffuseBlock);
        Reader.Convert(pColor, pSource->nVerticesCount);
        for (int i = 0; i < pSource->nVerticesCount; i++)
            pSource->pVertices[i].Color = pColor[i];
    }
    pSource->nVertexFVF |= FVF_COLOR;
    pSource->nVertexSize += sizeof(XMCOLOR);

    // TexCoords: XMFLOAT3->float2
    if (pHead->MeshHead.Blocks.TextureUVW1Block)
    {
        Reader.Seek(pHead->MeshHead.Blocks.TextureUVW1Block);
        Reader.Convert(pFloat3, pSource->nVerticesCount);
        for (int i = 0; i < pSource->nVerticesCount; i++)
            memcpy(&pSource->pVertices[i].TexCoords, &pFloat3[i], sizeof(XMFLOAT2));

        pSource->nVertexFVF |= FVF_TEX1;
        pSource->nVertexSize += sizeof(XMFLOAT2);
    }

    // Index
    if (pHead->MeshHead.Blocks.FacesIndexBlock)
    {
        pSource->pIndices = new DWORD[pSource->nIndexCount];

        Reader.Seek(pHead->MeshHead.Blocks.FacesIndexBlock);
        Reader.Copy(pSource->pIndices, pSource->nIndexCount);
    }

    // Subset Index
    if (pHead->MeshHead.Blocks.SubsetIndexBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.SubsetIndexBlock);

        if (bSubsetShort)
            _LoadSubsetIndex<WORD>(pData, &Reader, pSource->nFacesCount);
        else
            _LoadSubsetIndex<DWORD>(pData, &Reader, pSource->nFacesCount);

        _FillSubset(pSource, pData);
    }

    // Skin Info
    if (pHead->MeshHead.Blocks.SkinInfoBlock)
    {
        Reader.Seek(pHead->MeshHead.Blocks.SkinInfoBlock);
        _LoadBone(pSource, pData, &Reader, bHasPxPose, pHead->MeshHead.Blocks.BBoxBlock);
        _LoadSocket(pSource, pData, &Reader);

        pSource->nVertexFVF |= FVF_SKIN;
        pSource->nVertexSize += sizeof(float) * VERTEX_MAX_BONE;
        pSource->nVertexSize += sizeof(BYTE)* VERTEX_MAX_BONE;
    }
}

void _LoadBone(MESH_SOURCE* pSource, _MESH_FILE_DATA* pData, LBinaryReader* pReader, BOOL bHasPxPose, BOOL bHasBoundBox)
{
    int nRefVertexCount = 0;
    auto& Reader = *pReader;

    Reader.Copy(&pSource->nBonesCount);
    pSource->pBones = new BONE_SOURCE[pSource->nBonesCount];

    for (DWORD i = 0; i < pSource->nBonesCount; i++)
    {
        BONE_SOURCE& Bone = pSource->pBones[i];

        Reader.Copy(Bone.szName, 30);
        Reader.Seek(30, SEEK::CURSOR);

        Reader.Convert(Bone.nChildCount);
        if (Bone.nChildCount > 0)
        {
            Bone.pChildNames = new NAME_STRING[Bone.nChildCount];
            Reader.Copy(Bone.pChildNames, Bone.nChildCount);
        }

        Reader.Copy(&Bone.mOffset);
        Reader.Copy(&Bone.mOffset2Parent);

        if (bHasPxPose)
            Reader.Copy(&Bone.mInvPxPose);

        Reader.Copy(&nRefVertexCount);
        if (nRefVertexCount > 0)
        {
            DWORD* pVertexIndices = nullptr;
            float* pWeights = nullptr;

            Reader.Convert(pVertexIndices, nRefVertexCount);
            Reader.Convert(pWeights, nRefVertexCount);

            for (unsigned j = 0; j < nRefVertexCount; j++)
            {
                unsigned uVertexIndex = pVertexIndices[j];
                float fWeight = pWeights[j];

                assert(uVertexIndex < pSource->nVerticesCount);

                auto& BoneWeights = pSource->pVertices[uVertexIndex].BoneWeights;
                auto& BoneIndices = pSource->pVertices[uVertexIndex].BoneIndices;

                for (unsigned k = 0; k < VERTEX_MAX_BONE; k++)
                {
                    if (BoneIndices[k] == 0xFF)
                    {
                        BoneIndices[k] = (BYTE)i;
                        BoneWeights[k] = fWeight;
                        break;
                    }
                }
            }
        }

        if (bHasBoundBox)
        {
            Reader.Seek(sizeof(XMFLOAT4X4), SEEK::CURSOR);
            Reader.Copy(&Bone.BoundingBox);
            Reader.Seek(sizeof(BOOL), SEEK::CURSOR);
        }
    }
}

void _LoadSocket(MESH_SOURCE* pSource, _MESH_FILE_DATA* pData, LBinaryReader* pReader)
{
    auto& Reader = *pReader;

    Reader.Copy(&pSource->nSocketCount);
    if (pSource->nSocketCount > 0)
    {
        pSource->pSockets = new SOCKET_SOURCE[pSource->nSocketCount];

        Reader.Copy(pSource->pSockets, pSource->nSocketCount);
        std::sort(pSource->pSockets, pSource->pSockets + pSource->nSocketCount, [](SOCKET_SOURCE& left, SOCKET_SOURCE& right) {
            if (int nCmp = _stricmp(left.szName, right.szName))
                return nCmp < 0;
            return &left < &right;
            });
    }
}

void LoadMesh(MESH_DESC* pDesc, MESH_SOURCE*& pSource)
{
    pSource = new MESH_SOURCE;
    pSource->AddRef();

    _MESH_FILE_DATA MeshData = { 0 };

    _LoadMesh(pDesc->szName, pSource, &MeshData);
}