#include "LInput.h"
#include "LScene.h"
#include "LCamera.h"
#include "LAssert.h"

void LInput::ProcessInput(UINT& uMsg, WPARAM& wParam, LPARAM& lParam, LScene* pScene)
{
    BOOL_ERROR_EXIT(pScene);

    switch (uMsg)
    {
    case WM_MOUSEWHEEL:
    {
        pScene->m_pCamera->SetSightDistance(GET_WHEEL_DELTA_WPARAM(wParam) * -0.1f);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        m_nCursorX = LOWORD(lParam);
        m_nCursorY = HIWORD(lParam);

        m_bLButtonDown = true;
        break;
    }
    case WM_LBUTTONUP:
    {
        m_bLButtonDown = false;
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (m_bLButtonDown)
        {
            int nX = LOWORD(lParam);
            int nY = HIWORD(lParam);

            pScene->m_pCamera->AddYaw((m_nCursorX - nX) * m_fStepX);
            pScene->m_pCamera->AddPitch((nY - m_nCursorY) * m_fStepY);

            m_nCursorX = nX;
            m_nCursorY = nY;
        }
        break;
    }
    case WM_CHAR:
    {
        switch (wParam)
        {
        case 'q':
        case 'Q':
        {
            _CameraTargetUp(pScene->m_pCamera, m_fUpStep);
            break;
        }
        case 'e':
        case 'E':
        {
            _CameraTargetUp(pScene->m_pCamera, -m_fUpStep);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }

Exit0:
    return;
}

void LInput::_CameraTargetUp(LCamera* pCamera, float y)
{
    pCamera->MoveTarget(XMVectorSet(0, y, 0, 1.0f));
}
