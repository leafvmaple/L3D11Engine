#pragma once

#include "L3DInterface.h"
#include "L3DRenderUnit.h"

struct ID3DX11EffectTechnique;
struct ID3DX11EffectMatrixVariable;


namespace Colors
{
	XMGLOBALCONST XMFLOAT4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMFLOAT4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMFLOAT4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMFLOAT4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMFLOAT4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMFLOAT4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMFLOAT4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMFLOAT4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

	XMGLOBALCONST XMFLOAT4 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST XMFLOAT4 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}

class L3DBox : public L3DRenderUnit
{
public:
	HRESULT Create(ID3D11Device* piDevice);

	HRESULT Draw(ID3D11DeviceContext* pDeviceContext, XMMATRIX* pWorldViewProj) override;
	HRESULT PushRenderUnit(std::vector<RENDER_STAGE_INPUT_ASSEMBLER>& vecRenderUnit) override;

private:
	ID3DX11EffectTechnique* m_pEffectTech = nullptr;
	ID3DX11EffectMatrixVariable* m_pWorldViewProj = nullptr;
};