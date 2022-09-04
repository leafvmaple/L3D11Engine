#include "LClient.h"
#include "L3DInterface.h"

#include "Object/LCharacter.h"

LClient::LClient()
: m_fLastTime(0.f)
, m_fTimeElapsed(0.f)
, m_nFrame(0)
, m_pObjectMgr(NULL)
{

}

LClient::~LClient()
{

}

HRESULT LClient::Init(HINSTANCE hInstance)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    L3D_WINDOW_PARAM WindowParam;
    LScene* pCurScene = nullptr;
    LCharacter* pCharacter = nullptr;

    WindowParam.x = 100;
    WindowParam.y = 100;
    WindowParam.Width = 800;
    WindowParam.Height = 600;
    WindowParam.bWindow = TRUE;
    WindowParam.lpszClassName = TEXT("LDirect3D");
    WindowParam.lpszWindowName = TEXT("LDirect3D Engine");

    m_pObjectMgr = new LObjectMgr;
    BOOL_ERROR_EXIT(m_pObjectMgr);

    hr = m_pObjectMgr->Init(hInstance, WindowParam);
    HRESULT_ERROR_EXIT(hr);

    pCharacter = m_pObjectMgr->CreateModel<LCharacter>("Res/Mesh/A055_body.mesh");
    BOOL_ERROR_EXIT(pCharacter);

    m_fLastTime = (float)timeGetTime();

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT LClient::Update()
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    float fCurTime = 0;
    float fDeltaTime = 0;

    fCurTime = (float)timeGetTime();
    fDeltaTime = (fCurTime - m_fLastTime) * 0.001f;

    hr = m_pObjectMgr->Update(fDeltaTime);
    HRESULT_ERROR_EXIT(hr);

    hr = ShowFPS(fDeltaTime);
    HRESULT_ERROR_EXIT(hr);

    m_fLastTime = fCurTime;

    hResult = S_OK;
Exit0:
    return hResult;
}

void LClient::Uninit()
{
    if (m_pObjectMgr)
    {
        m_pObjectMgr->Uninit();
        SAFE_DELETE(m_pObjectMgr);
    }
}

BOOL LClient::IsActive()
{
    return m_pObjectMgr->IsActive();
}

HRESULT LClient::ShowFPS(float fDeltaTime)
{
    HRESULT hr = E_FAIL;
    HRESULT hResult = E_FAIL;
    WCHAR wszFPS[LENGIEN_FONT_STRING_MAX];

    m_nFrame++;
    m_fTimeElapsed += fDeltaTime;

    if (m_fTimeElapsed >= 1.0f)
    {
        swprintf(wszFPS, LENGIEN_FONT_STRING_MAX, TEXT("FPS:%.2f"), m_nFrame / m_fTimeElapsed);

        m_fTimeElapsed = 0.f;
        m_nFrame = 0;
    }
    hResult = S_OK;
Exit0:
    return hResult;
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    HRESULT hr = E_FAIL;
    LClient Client;

    hr = Client.Init(hInstance);
    HRESULT_ERROR_RETURN(hr);

    while (Client.IsActive())
    {
        hr = Client.Update();
        HRESULT_ERROR_BREAK(hr);
    }

    Client.Uninit();

    return 1;
}