#pragma once

#include "IMesh.h"
#include "L3DFormat.h"

struct LBinaryReader;

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

struct _BONE_INFO
{
    XMFLOAT4X4   mOffset2Parent;
    XMFLOAT4X4   mInvPxPose;
    L3D_BOUND_BOX BoundingBox;
};

struct _MESH_FILE_DATA
{
    DWORD dwVertexFVF = 0;
    std::vector<DWORD> SubsetIndices;
};

void _FillSubset(MESH_SOURCE* pSource, _MESH_FILE_DATA *pData);

void _LoadMesh(const char* szFileName, MESH_SOURCE* pSource, _MESH_FILE_DATA* pData);
void _LoadBone(MESH_SOURCE* pSource, _MESH_FILE_DATA* pData, LBinaryReader* pReader, BOOL bHasPxPose, BOOL bHasBoundBox);
void _LoadSocket(MESH_SOURCE* pSource, _MESH_FILE_DATA* pData, LBinaryReader* pReader);
