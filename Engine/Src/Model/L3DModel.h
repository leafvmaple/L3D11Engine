#pragma once

#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <memory>
#include <functional>
#include <filesystem>

#include "L3DMaterial.h"
#include "L3DInterface.h"

#include "Define/L3DSceneDefine.h"

#include "rapidjson/include/rapidjson/document.h"

class L3DMesh;
class L3DModel;
class L3DSkeleton;
class L3DMaterial;
class L3DRenderUnit;
class L3DTexture;
class L3DFlexible;
class L3DAnimationController;
// Temp
struct SCENE_RENDER_OPTION;
struct MESH_SHARED_CB;

struct ID3DX11EffectVariable;
struct ID3DX11EffectConstantBuffer;

enum BIND_TYPE
{
    BIND_NONE,
    BIND_TO_SOCKET,
    BIND_TO_BONE,
    BIND_TO_FLEXIBLE_BONE,
    ATTACH_TO_ACTOR,

};

struct L3D_BIND_EXTRA_INFO
{
    L3DModel* pModel = nullptr;
    unsigned int uBindIndex = 0;
};

struct L3D_BIND_INFO
{
    BIND_TYPE eBindType = BIND_NONE;
    std::string sBindTarget; // pcszBindToActorName 绑定的骨骼或插槽名
    L3D_BIND_EXTRA_INFO extraInfo;
};

class L3DModel : public ILModel
{
public:
    HRESULT Create(ID3D11Device* piDevice, const char* szFileName);

    void AttachActor(L3DModel* pModel);
    void BindToSocket(L3DModel* pModel, const char* szSocketName);

    void GetAllModel(std::vector<L3DModel*> &models);

    void GetWorldMatrix(XMFLOAT4X4& matrix);
    void GetSocketMatrix(int nSocketIndex, XMFLOAT4X4& matrix);

    virtual void AttachModel(ILModel* pModel) override;
    virtual void BindToSocket(ILModel* pModel, const char* szSocketNam) override;

    virtual HRESULT ResetTransform() override;
    virtual HRESULT SetTranslation(const XMFLOAT3& Translation) override;
    virtual HRESULT SetRotation(const XMFLOAT4& Rotation) override;
    virtual HRESULT SetScale(const XMFLOAT3& Scale) override;

    virtual HRESULT PlayAnimation(const char* szAnimation, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority) override;
    virtual HRESULT PlaySplitAnimation(const char* szAnimation, SPLIT_TYPE nSplitType, AnimationPlayType nPlayType, ANIMATION_CONTROLLER_PRIORITY nPriority) override;

    void PrimaryUpdate();
    void UpdateTransform();

    void UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption);
    void GetRenderUnit(SCENE_RENDER_QUEUES& RenderQueue);

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

    void _UpdateTransform();
    void _LoadMaterialFromJson(ID3D11Device* piDevice, const wchar_t* szFileName);

    void _UpdateModelSharedConsts(std::vector<XMMATRIX>& BoneMatrix, const MESH_SHARED_CB& MeshCB);
    void _UpdateSubsetConst(unsigned int iSubset);
    void _UpdateAnimation();
    void _UpdateBuffer();

    void _FrameMove();

    void _CreateBoneMatrix();
    
    void _UpdateModelVariablesIndices();

    void _InitModel(ID3D11Device* piDevice, const char* szFileName);
    void _InitSkeletion(ID3D11Device* piDevice, const char* szFileName);
    void _InitMesh(ID3D11Device* piDevice, const char* szFileName);
    void _InitRenderUnits();

    HRESULT _FindSocket(const char* szSocketName, L3D_BIND_EXTRA_INFO& BindExtraInfo);

    RENDER_DATA m_RenderData;

    L3DMesh* m_p3DMesh = nullptr;
    L3DSkeleton* m_pSkeleton = nullptr;
    L3DFlexible* m_pFlexible = nullptr;
    L3DAnimationController* m_p3DAniController[SPLIT_COUNT] = { nullptr };

    std::vector<XMMATRIX> m_BoneCurMatrix; // 当前骨骼的绝对变换矩阵
    std::vector<L3DModel*> m_ChildList;

    L3D_BIND_INFO m_BindInfo;

    std::filesystem::path m_Path;

    // m_vecMaterial Like MaterialInstancePack
    // L3DMaterial Like MaterialInstance
    MATERIALS_PACK m_MaterialPack;

    XMFLOAT3 m_Translation;
    XMFLOAT4 m_Rotation;
    XMFLOAT3 m_Scale;

    XMFLOAT4X4 m_World; // 当前模型的世界变换矩阵

    int m_nLOD = 0;
    int m_nGroup = 0;

    std::unordered_map<std::wstring, std::function<void(ID3D11Device* piDevice, const char* szFileName)>> m_InitFuncs = {
        std::make_pair(L".mdl", std::bind(&L3DModel::_InitModel, this, std::placeholders::_1, std::placeholders::_2)),
        std::make_pair(L".txt", std::bind(&L3DModel::_InitSkeletion, this, std::placeholders::_1, std::placeholders::_2)),
        std::make_pair(L".mesh", std::bind(&L3DModel::_InitMesh, this, std::placeholders::_1, std::placeholders::_2)),
    };
};
