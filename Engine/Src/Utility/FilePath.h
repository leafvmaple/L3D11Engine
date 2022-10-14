#pragma once

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#include <winerror.h>

namespace L3D
{
	HRESULT PathSplit(const char* szFileName, char* szRetDir, size_t nDirSize, char* szRetBase, size_t nBaseSize);

	HRESULT GetExtName(char* szFileName, char* szExt, size_t nExtSize);
	HRESULT FormatExtName(char* szFileName, const char* ExtName);
}