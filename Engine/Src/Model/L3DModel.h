#pragma once

#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <memory>
#include <functional>
#include <filesystem>

#include "L3DInterface.h"
#include "L3DMaterialDefine.h"

#include "rapidjson/include/rapidjson/document.h"

class L3DMesh;
class L3DSkeleton;
class L3DMaterial;
class L3DRenderUnit;
class L3DTexture;
class L3DAnmationController;

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

    void AttachActor(L3DModel* pModel);
    void GetAllModel(std::vector<L3DModel*> &models);

    virtual HRESULT ResetTransform() override;
    virtual HRESULT SetTranslation(const XMFLOAT3& Translation) override;
    virtual HRESULT SetRotation(const XMFLOAT4& Rotation) override;
    virtual HRESULT SetScale(const XMFLOAT3& Scale) override;

    virtual HRESULT PlayAnimation(const char* szAnimation, ANIMATION_PLAY_TYPE nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority) override;
    virtual HRESULT PlaySplitAnimation(const char* szAnimation, SPLIT_TYPE nSplitType, ANIMATION_PLAY_TYPE nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority) override;

    void PrimaryUpdate();

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
    void _UpdateAnimation();
    void _UpdateBuffer();

    void _FrameMove();

    void _CreateBoneMatrix();
    
    void _UpdateModelVariablesIndices();

    void _InitMdl(ID3D11Device* piDevice, const char* szFileName);
    void _InitSkeletion(ID3D11Device* piDevice, const char* szFileName);
    void _InitSingleModel(ID3D11Device* piDevice, const char* szFileName);
    void _InitRenderUnits();

    RENDER_DATA m_RenderData;

    L3DMesh* m_p3DMesh = nullptr;
    L3DSkeleton* m_pSkeleton = nullptr;
    L3DAnmationController* m_p3DAniController[SPLIT_COUNT] = { nullptr };

    std::vector<XMMATRIX> m_BoneCurMatrix;
    std::vector<L3DModel*> m_ChildList;

    std::filesystem::path m_Path;

    // m_vecMaterial Like MaterialInstancePack
    // L3DMaterial Like MaterialInstance
    MATERIALS_PACK m_MaterialPack;

    XMFLOAT3 m_Translation;
    XMFLOAT4 m_Rotation;
    XMFLOAT3 m_Scale;

    XMFLOAT4X4 m_World;

    std::unordered_map<std::wstring, std::function<void(ID3D11Device* piDevice, const char* szFileName)>> m_InitFuncs = {
        std::make_pair(L".mdl", std::bind(&L3DModel::_InitMdl, this, std::placeholders::_1, std::placeholders::_2)),
        std::make_pair(L".txt", std::bind(&L3DModel::_InitSkeletion, this, std::placeholders::_1, std::placeholders::_2)),
        std::make_pair(L".mesh", std::bind(&L3DModel::_InitSingleModel, this, std::placeholders::_1, std::placeholders::_2)),
    };
};
