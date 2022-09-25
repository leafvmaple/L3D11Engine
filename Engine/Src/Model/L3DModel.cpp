#include "stdafx.h"

#include "L3DModel.h"
#include "L3DMesh.h"
#include "L3DMaterial.h"

#include "Render/L3DRenderUnit.h"

HRESULT L3DModel::Create(ID3D11Device* piDevice, const char* szFileName)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;

    RUNTIME_MACRO eStaticMacro = RUNTIME_MACRO_MESH;

    m_pMesh = new L3DMesh;
    BOOL_ERROR_EXIT(m_pMesh);

    hr = m_pMesh->Create(piDevice, szFileName);
    HRESULT_ERROR_EXIT(hr);

    if (m_pMesh->m_dwBoneCount > 0)
        eStaticMacro = RUNTIME_MACRO_SKIN_MESH;

    m_pMaterial = new L3DMaterial;
    BOOL_ERROR_EXIT(m_pMaterial);

    hr = m_pMaterial->Create(piDevice, eStaticMacro);
    HRESULT_ERROR_EXIT(hr);

    m_pRenderUnit = new L3DRenderUnit;
    BOOL_ERROR_EXIT(m_pRenderUnit);

    hr = m_pRenderUnit->Create(m_pMesh->m_Stage, m_pMaterial);
    HRESULT_ERROR_EXIT(hr);

    hResult = S_OK;
Exit0:
    return hResult;
}
