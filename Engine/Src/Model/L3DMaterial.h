#pragma once

enum RUNTIME_MACRO
{
	RUNTIME_MACRO_MESH = 0,
	RUNTIME_MACRO_SKIN_MESH = 1,
};

class L3DMaterial
{

};

L3DMaterial* LoadMaterial(ID3D11Device* piDevice, RUNTIME_MACRO eMacro, const char* szFile);