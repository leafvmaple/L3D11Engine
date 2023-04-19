#pragma once

#include <d3d11.h>
#include <vector>

#include "L3DInterface.h"

struct SCENE_PATH_TABLE;
struct SCENE_RENDER_OPTION;

class L3DEnvironment;
class L3DRenderUnit;
class L3DModel;

typedef std::vector <L3DModel*> MODEL_VECTOR;

class L3DScene : public ILScene
{
public:
	HRESULT Create(ID3D11Device* piDevice, const char* szFileName);
	HRESULT SetParent(IL3DEngine* piEngine);

	void BeginRender(const SCENE_RENDER_OPTION& RenderOption);
	void EndRender(const SCENE_RENDER_OPTION& RenderOption);

	HRESULT AddRenderEntity(ILModel* piModel) override;

	void GetVisibleObjectAll(MODEL_VECTOR& vecModels);

private:
	struct SCENE_RENDER_QUEUES
	{
		std::vector<L3DRenderUnit*> GBuffer;
	};

	HRESULT _CreatePathTable(const char* szFileName, SCENE_PATH_TABLE* pPathTable);

	HRESULT _LoadLogicalSceneRect(const char* szSettingName, L3D_SCENE_RECT* pRect);
	HRESULT _LoadEnvironmentSetting(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);

	void _UpdateVisibleList();
	void _RenderMainCamera(const SCENE_RENDER_OPTION& RenderOption);

	MODEL_VECTOR m_DynamicalObjects;
	MODEL_VECTOR m_VisibleModel;

	SCENE_RENDER_QUEUES m_RenderQueues;

	IL3DEngine* m_piEngine;
	L3DEnvironment* m_pEnvironment = nullptr;
};