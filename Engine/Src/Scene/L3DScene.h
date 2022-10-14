#pragma once

#include <d3d11.h>
#include <vector>

#include "L3DInterface.h"

struct SCENE_PATH_TABLE;
struct SCENE_RENDER_OPTION;

class L3DEnvironment;
class L3DRenderUnit;
class L3DModel;

class L3DScene : public ILScene
{
public:
	HRESULT Create(ID3D11Device* piDevice, const char* szFileName);
	HRESULT SetParent(IL3DEngine* piEngine);

	HRESULT Update(const SCENE_RENDER_OPTION& RenderOption);

	HRESULT AddRenderEntity(ILModel* piModel) override;

private:
	HRESULT _CreatePathTable(const char* szFileName, SCENE_PATH_TABLE* pPathTable);

	HRESULT _LoadLogicalSceneRect(const char* szSettingName, L3D_SCENE_RECT* pRect);
	HRESULT _LoadEnvironmentSetting(ID3D11Device* piDevice, const SCENE_PATH_TABLE& pathTable);

	std::vector<L3DRenderUnit*> m_vecIAStage;

	IL3DEngine* m_piEngine;
	L3DEnvironment* m_pEnvironment = nullptr;
};