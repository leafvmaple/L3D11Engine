#pragma once

#include <d3d11.h>
#include <vector>

#include "L3DInterface.h"
#include "Define/L3DSceneDefine.h"
#include "Model/L3DMaterialConfig.h"

struct SCENE_PATH_TABLE;
struct SCENE_RENDER_OPTION;
struct RENDER_TO_TARGE_PARAM;

class L3DEnvironment;
class L3DRenderUnit;
class L3DLandscape;
class L3DCamera;
class L3DModel;

struct L3DLogicalConstances
{
    FLOAT nLogicalCellLength; // 一个Cell的长度，单位为厘米
    FLOAT nCellLength; // 一个Cell的逻辑长度

    FLOAT nPointPerAltitude; // 一个高度单位的逻辑长度
    FLOAT nAltitudeUnit; // 一个高度单位的长度，单位为厘米
};

struct L3D_LOGICAL
{
    float nSceneX_Start = 0;
    float nSceneZ_Start = 0;
    float nSceneX_Width = 0;
    float nSceneZ_Width = 0;

    L3DLogicalConstances Constances;
};

typedef std::vector<L3DModel*> MODEL_VECTOR;

class L3DScene : public ILScene
{
public:
    HRESULT Create(ID3D11Device* piDevice, const char* szFileName);
    HRESULT SetParent(IL3DEngine* piEngine);

    void BeginRender(const SCENE_RENDER_OPTION& RenderOption);
    void EndRender(const SCENE_RENDER_OPTION& RenderOption);

    void Update(const SCENE_RENDER_OPTION& RenderOption);

    void GetVisibleObjectAll(MODEL_VECTOR& vecModels);
    
    L3DCamera* GetCamera() { return m_pCamera; }
    void UpdateCamera();

    void AddRenderEntity(ILModel* piModel) override;

    void GetFloor(const XMFLOAT3& vPos, float& fHeight) override;
    void LogicalToScene(XMFLOAT3& vPos, int nX, int nY, int nZ) override;

    virtual ILCamera* Get3DCamera();

private:
    void _CreateCamera();
    HRESULT _CreatePathTable(const char* szFileName, SCENE_PATH_TABLE* pPathTable);
    void _CreateScene(const char* szFileName);

    // _GetLogicalSceneRect
    HRESULT _LoadLogicalSceneRect(const wchar_t* szSettingName);
    HRESULT _LoadEnvironmentSetting(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);
    HRESULT _LoadLandscape(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);

    void _UpdateVisibleList();
    void _UpdateCommonRenderData(const SCENE_RENDER_OPTION& RenderOption);
    void _UpdateCommonTextures(const SCENE_RENDER_OPTION& RenderOption);

    void _CommitConstData(const SCENE_RENDER_OPTION& RenderOption);
    void _UpdateCommonConstData(const SCENE_RENDER_OPTION& RenderOption);

    void _RenderMainCamera(const SCENE_RENDER_OPTION& RenderOption);

    MODEL_VECTOR m_DynamicalObjects;
    MODEL_VECTOR m_VisibleModel;

    SCENE_RENDER_QUEUES m_RenderQueues;
    SCENE_RENDER_CONTEXT m_RenderContext;

    SHARED_SHADER_COMMON m_ShaderCommonParam;

    L3D_LOGICAL m_Logical;

    IL3DEngine* m_piEngine = nullptr;
    L3DCamera* m_pCamera = nullptr;
    L3DEnvironment* m_pEnvironment = nullptr;
    L3DLandscape* m_pLandscape = nullptr;
};