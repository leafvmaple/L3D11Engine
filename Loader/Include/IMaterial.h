#pragma once

#include "LInterface.h"
#include "LAssert.h"

struct MATERIAL_DESC
{
    const wchar_t* szFileName = nullptr;
};

struct MATERIAL_DEFINE
{
    char szName[MAX_PATH] = { 0 };
    char szShaderName[MAX_PATH] = { 0 };

    unsigned int nParam = 0;
    struct _Param
    {
        char szName[MAX_PATH]     = { 0 };
        char szRegister[MAX_PATH] = { 0 };
        union
        {
            char szValue[MAX_PATH] = { 0 };
            float fValue;
        };
    }* pParam = nullptr;

    ~MATERIAL_DEFINE()
    {
        SAFE_DELETE_ARRAY(pParam);
    }
};

struct MATERIAL_SOURCE_SUBSET
{
    unsigned int nBlendMode   = 0;
    unsigned int nAlphaRef    = 0;
    unsigned int nAlphaRef2   = 0;
    unsigned int nTwoSideFlag = 0;

    unsigned int nTexture = 0;
    struct _Param
    {
        char szName[MAX_PATH]  = { 0 };
        char szType[MAX_PATH]  = { 0 };
        char szValue[MAX_PATH] = { 0 };
    }* pTexture = nullptr;

    MATERIAL_DEFINE Define;

    ~MATERIAL_SOURCE_SUBSET() {
        SAFE_DELETE_ARRAY(pTexture);
    }
};

struct MATERIAL_SOURCE_GROUP
{
    unsigned int nSubset = 0;
    MATERIAL_SOURCE_SUBSET* pSubset = nullptr;

    ~MATERIAL_SOURCE_GROUP() {
        SAFE_DELETE_ARRAY(pSubset);
    }
};

struct MATERIAL_SOURCE_LOD
{
    unsigned int nGroup = 0;
    MATERIAL_SOURCE_GROUP* pGroup = nullptr;

    ~MATERIAL_SOURCE_LOD() {
        SAFE_DELETE_ARRAY(pGroup);
    }
};

struct MATERIAL_SOURCE : LUnknown
{
    unsigned int nLOD = 0;
    MATERIAL_SOURCE_LOD* pLOD = nullptr;

    virtual ~MATERIAL_SOURCE() {
        SAFE_DELETE_ARRAY(pLOD);
    }
};

L3DENGINE_API void LoadMaterial(MATERIAL_DESC* pDesc, MATERIAL_SOURCE*& pSource);