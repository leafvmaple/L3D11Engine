#pragma once

#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>

#include "L3DInterface.h"

#include "rapidjson/include/rapidjson/document.h"

class L3DMesh;
class L3DMaterial;
class L3DRenderUnit;
class L3DTexture;

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

    void UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption);
    void GetRenderUnit(std::vector<L3DRenderUnit*>& RenderQueue);

    L3DRenderUnit* m_pRenderUnit = nullptr;

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
    };

    void UpdateTransFrom();
    HRESULT _LoadMaterialFromJson(ID3D11Device* piDevice, const char* szFileName);
    HRESULT _LoadInstanceFromJson(rapidjson::Value& JsonObject, MATERIAL_INSTANCE_DATA& InstanceData);
    HRESULT _LoadSubsetMaterial(ID3D11Device* piDevice, MATERIAL_INSTANCE_DATA& InstanceData);

    void _UpdateModelSharedConsts(std::vector<XMMATRIX>& BoneMatrix, const MESH_SHARED_CB& MeshCB);

    void _CreateBoneMatrix();
    void _InitRenderData();
    void _InitRenderUnits();

    RENDER_DATA m_RenderData;

    L3DMesh* m_p3DMesh = nullptr;

    std::vector<XMMATRIX> m_BoneCurMatrix;
    std::vector<L3DMaterial*> m_vecMaterial;

    XMFLOAT3 m_Translation;
    XMFLOAT4 m_Rotation;
    XMFLOAT3 m_Scale;

    XMFLOAT4X4 m_World;
};