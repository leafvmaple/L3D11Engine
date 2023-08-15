#include "LInput.h"
#include "LScene.h"
#include "LCamera.h"

void LInput::ProcessInput(UINT& uMsg, WPARAM& wParam, LPARAM& lParam, LScene* pScene)
{
	switch (uMsg)
	{
    case WM_MOUSEWHEEL:
    {
        if (pScene)
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
        if (m_bLButtonDown && pScene)
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
	default:
		break;
	}
}
