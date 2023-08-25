#pragma once

#include <unknwn.h>
#include <assert.h>

#ifdef LENGINE_EXPORTS
#define L3DENGINE_API extern "C" __declspec(dllexport)
#define L3DENGINE_CLASS __declspec(dllexport)
#else
#define L3DENGINE_API extern "C" __declspec(dllimport)
#define L3DENGINE_CLASS __declspec(dllimport)
#endif

#define COM_RELEASE(obj) \
{                       \
    assert(nCount);     \
    if (!(--nCount))    \
        delete obj;     \
}

#define COM_RELEASE_ARRAY(obj) \
{                       \
    assert(nCount);     \
    if (!(--nCount))    \
        delete[] obj;   \
}

struct LUnknown : IUnknown
{
    int nCount = 0;

    virtual ULONG STDMETHODCALLTYPE AddRef() override { ++nCount; return 0; };
    virtual ULONG STDMETHODCALLTYPE Release() override { COM_RELEASE(this); return 0; };
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_  void** ppvObject) override { return S_OK; };
};