#pragma once

#include <assert.h>

#define BOOL_ERROR_RETURN(Condition) \
    if (!(Condition)) {              \
        return 0;                    \
    }

#define BOOL_ERROR_LOG_RETURN(Condition, LOG, ...)   \
    if (!(Condition)) {                              \
        printf(LOG, __VA_ARGS__);                    \
        return 0;                                    \
    }

#define BOOL_ERROR_CONTINUE(Condition)   \
    if (!(Condition)) {                  \
        continue;                        \
    }

#define BOOL_ERROR_BREAK(Condition)   \
    if (!(Condition)) {               \
        break;                        \
    }

#define BOOL_ERROR_EXIT(Condition)   \
    if (!(Condition)) {               \
        goto Exit0;                        \
    }

#define BOOL_SUCCESS_BREAK(Condition) \
    if ((Condition)) {                \
        hResult = S_OK;               \
        break;                        \
    }

#define BOOL_SUCCESS_EXIT(Condition) \
    if ((Condition)) {                \
        hResult = S_OK;               \
        goto Exit0;                        \
    }

#define HRESULT_ERROR_RETURN(hr) \
    if (!SUCCEEDED(hr)) {        \
        return 0;                \
    }

#define HRESULT_ERROR_LOG_RETURN(hr, LOG, ...)   \
    if (!SUCCEEDED(hr)) {                        \
        printf(LOG, __VA_ARGS__);                \
        return 0;                                \
    }

#define HRESULT_ERROR_CONTINUE(hr)   \
    if (!SUCCEEDED(hr)) {            \
        continue;                    \
    }

#define HRESULT_ERROR_BREAK(hr)   \
    if (!SUCCEEDED(hr)) {         \
        break;                    \
    }

#define HRESULT_ERROR_EXIT(hr)    \
    if (!SUCCEEDED(hr)) {         \
        goto Exit0;                    \
    }

#define HRESULT_SUCCESS_BREAK(hr) \
    if (SUCCEEDED(hr)) {          \
        hResult = S_OK;           \
        break;                    \
    }

#define HRESULT_SUCCESS_EXIT(hr) \
    if (SUCCEEDED(hr)) {         \
        hResult = S_OK;          \
        goto Exit0;              \
    }

#define SAFE_FREE(p)    \
    if ((p)) {          \
        free((p));      \
        (p) = nullptr;  \
    }

#define SAFE_DELETE(p) \
    if ((p)) {         \
        delete (p);    \
        (p) = nullptr; \
    }

#define SAFE_RELEASE(p) \
    if ((p)) {          \
        (p)->Release(); \
        (p) = nullptr;  \
    }

#define SAFE_CLOSE_HANDLE(h)    \
    if ((h)) {                  \
        CloseHandle((h));       \
        (h) = nullptr;          \
    }
