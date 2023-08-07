#pragma once

#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <memory>

#include "L3DInterface.h"
#include "L3DMaterialDefine.h"

#include "rapidjson/include/rapidjson/document.h"

class L3DMesh;
class L3DMaterial;
class L3DRenderUnit;
class L3DTexture;
class L3DAnimation;

struct MATERIAL_INSTANCE_DATA;
// Temp
struct SCENE_RENDER_OPTION;
struct MESH_SHARED_CB;

struct ID3DX11EffectVariable;
struct ID3DX11EffectConstantBuffer;

class L3DModel : public ILModel
{
public:
    HRESULT Create(ID3D11Device* piDevice, const char* szFileName);

    virtual HRESULT ResetTransform() override;
    virtual HRESULT SetTranslation(const XMFLOAT3& Translation) override;
    virtual HRESULT SetRotation(const XMFLOAT4& Rotation) override;
    virtual HRESULT SetScale(const XMFLOAT3& Scale) override;

    virtual HRESULT PlayAnimation(const char* szAnimation) override;

    void UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption);
    void GetRenderUnit(std::vector<L3DRenderUnit*>& RenderQueue);

private:
    struct MODEL_FIX_VARIBLES
    {
        ID3DX11EffectVariable* pCustomMatrixBones = nullptr;
        ID3DX11EffectVariable* pModelParams = nullptr;
    };
    struct RENDER_DATA
    {
        ID3DX11EffectConstantBuffer* piModelSharedCB = nullptr;
        MODEL_FIX_VARIBLES ModelVariables;

        std::vector<ID3DX11EffectConstantBuffer*> SubsetCB;

        std::vector<L3DRenderUnit> RenderUnits;
    };

    void UpdateTransFrom();
    void _LoadMaterialFromJson(ID3D11Device* piDevice, const char* szFileName);

    void _UpdateModelSharedConsts(std::vector<XMMATRIX>& BoneMatrix, const MESH_SHARED_CB& MeshCB);
    void _UpdateSubsetConst(unsigned int iSubset);

    void _CreateBoneMatrix();
    
    void _UpdateModelVariablesIndices();

    void _InitRenderUnits();

    RENDER_DATA m_RenderData;

    L3DMesh* m_p3DMesh = nullptr;
    L3DAnimation* m_p3DAnimation = nullptr;

    std::vector<XMMATRIX> m_BoneCurMatrix;

    // m_vecMaterial Like MaterialInstancePack
    // L3DMaterial Like MaterialInstance
    MATERIALS_PACK m_MaterialPack;

    XMFLOAT3 m_Translation;
    XMFLOAT4 m_Rotation;
    XMFLOAT3 m_Scale;

    XMFLOAT4X4 m_World;
};