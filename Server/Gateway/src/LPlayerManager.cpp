#include "LAssert.h"
#include "LPlayerManager.h"

#define MAX_WAIT_ACCEPT     16
#define MAX_SOCKET_EVENT    (1024 + MAX_WAIT_ACCEPT)

int LPlayerManager::Init()
{
	int nRetCode = false;

	nRetCode = m_pSocketServer.Init("127.0.0.1", 3724, MAX_WAIT_ACCEPT, 1024 * 4, 1024 * 64, KSG_ENCODE_DECODE, nullptr);
	BOOL_ERROR_EXIT(nRetCode);

Exit0:
	return nRetCode;
}

void LPlayerManager::Uninit()
{
	m_pSocketServer.Uninit();
}

void LPlayerManager::Activate()
{

}
